#include "store.h"
#include "csapp.h"
#include "debug.h"

struct the_map store;

/*
 * Initialize the store.
 */
void store_init(void) {
    debug("Initialize object store");
}

/*
 * Finalize the store.
 */
void store_fini(void) {
    debug("Finalize object store");
}

/*
 * Put a key/value mapping in the store.  The key must not be NULL.
 * The value may be NULL, in which case this operation amounts to
 * deleting any existing mapping for the given key.
 *
 * This operation inherits the key and consumes one reference on
 * the value.
 *
 * @param tp        The transaction in which the operation is being performed.
 * @param key       The key.
 * @param value     The value.
 *
 * @return          Updated status of the transation, either TRANS_PENDING,
 *                  or TRANS_ABORTED.  The purpose is to be able to avoid
 *                  doing further operations in an already aborted transaction.
 */
TRANS_STATUS store_put(TRANSACTION *tp, KEY *key, BLOB *value) {
    return -1;
}

/*
 * Get the value associated with a specified key.  A pointer to the
 * associated value is stored in the specified variable.
 *
 * This operation inherits the key.  The caller is responsible for
 * one reference on any returned value.
 *
 * @param tp        The transaction in which the operation is being performed.
 * @param key       The key.
 * @param valuep    A variable into which a returned value pointer may be
 *                  stored.  The value pointer store may be NULL, indicating
 *                  that there is no value currently associated in the store
 *                  with the specified key.
 *
 * @return          Updated status of the transation, either TRANS_PENDING,
 *                  or TRANS_ABORTED.  The purpose is to be able to avoid
 *                  doing further operations in an already aborted transaction.
 */
TRANS_STATUS store_get(TRANSACTION *tp, KEY *key, BLOB **valuep) {
    return -1;
}

/*
 * Print the contents of the store to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 */
void store_show(void) {
    fprintf(stderr, "CONTENTS OF STORE:\n");
    for(int i = 0; i<NUM_BUCKETS; i++) {
        fprintf(stderr, "%d:\t", i);
        fprintf(stderr, "{key: %p [%s], ", store.key, store->key->blob->content);
        fprintf(stderr, "versions: ");
        fprintf(stderr, "{creator=%d (committed), blob=%p [%d]}}", );
    }
//
// 0:
// 1:
//     {key: 0x5423c80 [abc], versions: {creator=0 (committed), (NULL blob)}{creator=3 (committed), (NULL blob)}}
//     {key: 0x54235a0 [1], versions: {creator=0 (committed), blob=0x54235f0 [2]}}
// 2:
// 3:
// 4:
// 5:
// 6:
// 7:

}

// typedef struct map_entry {
//     KEY *key;
//     VERSION *versions;
//     struct map_entry *next;
// } MAP_ENTRY;

// struct map {
//     MAP_ENTRY **table;      // The hash table.
//     int num_buckets;        // Size of the table.
//     pthread_mutex_t mutex;  // Mutex to protect the table.
// } the_map;