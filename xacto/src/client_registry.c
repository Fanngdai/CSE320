#include "client_registry.h"
#include "csapp.h"
#include "debug.h"

#include <linux/fd.h>
#include <sys/select.h>
#include <sys/socket.h>

/*
 * A client registry keeps track of the file descriptors for clients
 * that are currently connected.  Each time a client connects,
 * its file descriptor is added to the registry.  When the thread servicing
 * a client is about to terminate, it removes the file descriptor from
 * the registry. The client registry also provides a function for shutting
 * down all client connections and a function that can be called by a thread
 * that wishes to wait for the client count to drop to zero.  Such a function
 * is useful, for example, in order to achieve clean termination:
 * when termination is desired, the "main" thread will shut down all client
 * connections and then wait for the set of registered file descriptors to
 * become empty before exiting the program.
 */
typedef struct client_registry {
    fd_set fds;
    pthread_mutex_t mutex;
    sem_t sem;
    uint32_t val;
} CLIENT_REGISTRY;

/*
 * Initialize a new client registry.
 *
 * @return  the newly initialized client registry.
 */
CLIENT_REGISTRY *creg_init() {
    debug("Initialize client registry");
    CLIENT_REGISTRY *cr = Calloc(sizeof(CLIENT_REGISTRY), sizeof(char));

    if(cr) {
        // If calloced success, initialize mutex & sem
        if(pthread_mutex_init(&cr->mutex, NULL) < 0
            || sem_init(&cr->sem, 0, 0) < 0) {
            Free(cr);
            return NULL;
        }
        // Set file descriptors to zero
        FD_ZERO(&cr->fds);
    }

    return cr;
}


/* Finalize a client registry.
 *
 * @param cr  The client registry to be finalized, which must not
 * be referenced again.
 */
void creg_fini(CLIENT_REGISTRY *cr) {
    if(!cr) return;
    pthread_mutex_destroy(&cr->mutex);
    sem_destroy(&cr->sem);
    Free(cr);
}

/*
 * Register a client file descriptor.
 *
 * @param cr  The client registry.
 * @param fd  The file descriptor to be registered.
 */
void creg_register(CLIENT_REGISTRY *cr, int fd) {
    if(!cr || pthread_mutex_lock(&cr->mutex) < 0)  return;
    debug("Register client %d (total connected: %d)", fd, cr->val);

    cr->val++;
    FD_SET(fd, &cr->fds);

    if(pthread_mutex_unlock(&cr->mutex) < 0) return;
}

/*
 * Unregister a client file descriptor, alerting anybody waiting
 * for the registered set to become empty.
 *
 * @param cr  The client registry.
 * @param fd  The file descriptor to be unregistered.
 */
void creg_unregister(CLIENT_REGISTRY *cr, int fd) {
    if(!cr || pthread_mutex_lock(&cr->mutex) < 0) return;
    debug("Unregister client %d (total connected: %d)", fd, cr->val);

    cr->val--;
    FD_CLR(fd, &cr->fds);

    if(pthread_mutex_unlock(&cr->mutex) < 0) return;
    if(cr->val == 0) V(&cr->sem);
}

/*
 * A thread calling this function will block in the call until
 * the number of registered clients has reached zero, at which
 * point the function will return.
 *
 * @param cr  The client registry.
 */
void creg_wait_for_empty(CLIENT_REGISTRY *cr) {
    if(!cr) return;
    // Wrapper for sem_wait. Wait till all threads are done
    P(&cr->sem);
}

/*
 * Shut down all the currently registered client file descriptors.
 *
 * @param cr  The client registry.
 */
void creg_shutdown_all(CLIENT_REGISTRY *cr) {
    if(!cr) return;
    for(int i = 0; i < FD_SETSIZE; i++)
        if(FD_ISSET(i, &cr->fds))
            shutdown(i, SHUT_RD);
}