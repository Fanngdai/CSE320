#include "transaction.h"
#include "csapp.h"
#include "debug.h"

unsigned int unique_id = 0;

/*
 * Initialize the transaction manager.
 */
void trans_init(void) {
    debug("Initialize transaction manager");
    // Transaction ID. Start at the largest unsigned number
    trans_list.id = -1;
    // Number of references (pointers) to transaction.
    trans_list.refcnt = 0;
    // Current transaction status.
    trans_list.status = TRANS_PENDING;
    // Singly-linked list of dependencies.
    trans_list.depends = NULL;
    // Number of transactions waiting for this one.
    trans_list.waitcnt = 0;
    // Semaphore to wait for transaction to commit or abort.
    sem_init(&trans_list.sem, 0, 0);
    // Mutex to protect fields.
    pthread_mutex_init(&trans_list.mutex, NULL);

    trans_list.prev = &trans_list;
    trans_list.next = &trans_list;
}

/*
 * Finalize the transaction manager.
 */
void trans_fini(void) {
    debug("Finalize transaction manager");
}

/*
 * Create a new transaction.
 *
 * @return                  A pointer to the new transaction
 *                          (with reference count 1) is returned if creation is
 *                          successful, otherwise NULL is returned.
 */
TRANSACTION *trans_create(void) {
    TRANSACTION *tp = Calloc(sizeof(TRANSACTION), sizeof(char));

    if(tp) {
        // If calloced success, initialize mutex & sem
        if(pthread_mutex_init(&tp->mutex, NULL) < 0
            || sem_init(&tp->sem, 0, 0) < 0) {
            Free(tp);
            tp = NULL;
            return NULL;
        }
        // Transaction ID.
        tp->id = unique_id++;
        // Number of references (pointers) to transaction.
        trans_ref(tp, "for newly created transaction");
        // Current transaction status.
        tp->status = TRANS_PENDING;
        // Singly-linked list of dependencies.
        tp->depends = NULL;
        // Number of transactions waiting for this one.
        tp->waitcnt = 0;

        tp->prev = trans_list.prev;
        tp->next = &trans_list;
        trans_list.prev->next = tp;
        trans_list.prev = tp;
        debug("Create new transaction %d", unique_id);
    }
    return tp;
}

/*
 * Increase the reference count on a transaction.
 *
 * @param tp                The transaction.
 * @param why               Short phrase explaining the purpose of the increase.
 *
 * @return                  The transaction pointer passed as the argument.
 */
TRANSACTION *trans_ref(TRANSACTION *tp, char *why) {
    if(!tp || pthread_mutex_lock(&tp->mutex) < 0) return NULL;
    tp->refcnt++;
    if(pthread_mutex_unlock(&tp->mutex) < 0) return NULL;
    if(why) debug("Increase ref count on transaction %d (%d -> %d) %s", tp->id, tp->refcnt-1, tp->refcnt, why);
    return tp;
}

/*
 * Decrease the reference count on a transaction.
 * If the reference count reaches zero, the transaction is freed.
 *
 * @param tp                The transaction.
 * @param why               Short phrase explaining the purpose of the decrease.
 */
void trans_unref(TRANSACTION *tp, char *why) {
    if(!tp || pthread_mutex_lock(&tp->mutex) < 0) return;
    if(why) debug("Decrease ref count on transaction %d (%d -> %d) %s", tp->id, tp->refcnt+1, tp->refcnt, why);
    tp->refcnt--;
    if(pthread_mutex_unlock(&tp->mutex) < 0) return;
    if(tp->refcnt == 0) {
        pthread_mutex_destroy(&tp->mutex);
        sem_destroy(&tp->sem);

        DEPENDENCY *dependent = tp->depends;
        while(dependent) {
            DEPENDENCY *temp = dependent->next;
            Free(dependent);
            dependent = temp;
        }

        tp->next->prev = tp->prev;
        tp->prev->next = tp->next;

        Free(tp);
        tp = NULL;
    }
}

/*
 * Add a transaction to the dependency set for this transaction.
 *
 * @param tp                The transaction to which the dependency is being added.
 * @param dtp               The transaction that is being added to the dependency set.
 */
void trans_add_dependency(TRANSACTION *tp, TRANSACTION *dtp) {
        if(!tp || !dtp || pthread_mutex_lock(&tp->mutex) < 0) return;
    debug("Release %d waiters dependent on transaction %d", dtp->waitcnt, dtp->id);

    DEPENDENCY *depend = Calloc(sizeof(DEPENDENCY), sizeof(char));
    depend->trans = dtp;
    depend->next = NULL;

    if(tp->depends) {
        DEPENDENCY *d_cursor = tp->depends;
        // Keep going until the next is NULL
        while(d_cursor->next) {
            if(d_cursor->trans->id == dtp->id) {
                if(depend) {
                    Free(depend);
                    depend = NULL;
                }
                pthread_mutex_unlock(&tp->mutex);
                return;
            }
            d_cursor = d_cursor->next;
        }
        d_cursor->next = depend;
    } else {
        tp->depends = depend;
    }

    trans_ref(dtp, NULL);
    pthread_mutex_unlock(&tp->mutex);
}

/*
 * Try to commit a transaction.  Committing a transaction requires waiting
 * for all transactions in its dependency set to either commit or abort.
 * If any transaction in the dependency set abort, then the dependent
 * transaction must also abort.  If all transactions in the dependency set
 * commit, then the dependent transaction may also commit.
 *
 * In all cases, this function consumes a single reference to the transaction
 * object.
 *
 * Does not check tp is NULL bc I wouldn't know what to return...
 *
 * @param tp    The transaction to be committed.
 * @return      The final status of the transaction: either TRANS_ABORTED,
 *              or TRANS_COMMITTED.
 */
TRANS_STATUS trans_commit(TRANSACTION *tp) {
    debug("Transaction %d trying to commit", tp->id);

    // P on all the dependencies
    DEPENDENCY *depend = tp->depends;
    while(depend) {
        pthread_mutex_lock(&tp->mutex);
        int status = depend->trans->status;
        pthread_mutex_unlock(&tp->mutex);

        if(status == TRANS_PENDING) {
            pthread_mutex_lock(&tp->mutex);
            depend->trans->waitcnt++;
            pthread_mutex_unlock(&tp->mutex);
            P(&depend->trans->sem);
        }
        depend = depend->next;
    }

    depend = tp->depends;
    while(depend) {
        if(depend->trans->status == TRANS_ABORTED) {
            return trans_abort(tp);
        }
        depend = depend->next;
    }

    pthread_mutex_lock(&tp->mutex);
    int waitcnt = tp->waitcnt;
    pthread_mutex_unlock(&tp->mutex);

    // wait for the wait count and V
    while(waitcnt) {
        V(&tp->sem);
        pthread_mutex_lock(&tp->mutex);
        waitcnt--;
        pthread_mutex_unlock(&tp->mutex);
    }

    // lock and status commit
    pthread_mutex_lock(&tp->mutex);
    tp->status = TRANS_COMMITTED;
    pthread_mutex_unlock(&tp->mutex);

    trans_unref(tp, NULL);
    return TRANS_COMMITTED;
}


/*
 * Abort a transaction.  If the transaction has already committed, it is
 * a fatal error and the program crashes.  If the transaction has already
 * aborted, no change is made to its state.  If the transaction is pending,
 * then it is set to the aborted state, and any transactions dependent on
 * this transaction must also abort.
 *
 * In all cases, this function consumes a single reference to the transaction
 * object.
 *
 * @param tp    The transaction to be aborted.
 * @return      TRANS_ABORTED.
 */
TRANS_STATUS trans_abort(TRANSACTION *tp) {
    if(tp->status == TRANS_PENDING) {
        pthread_mutex_lock(&tp->mutex);
        tp->status = TRANS_ABORTED;
        pthread_mutex_unlock(&tp->mutex);
    } else if(tp->status == TRANS_COMMITTED) {
        trans_unref(tp, NULL);
        abort();
    }

    pthread_mutex_lock(&tp->mutex);
    int waitcnt = tp->waitcnt;
    pthread_mutex_unlock(&tp->mutex);

    // wait for the wait count and V
    while(waitcnt) {
        V(&tp->sem);
        pthread_mutex_lock(&tp->mutex);
        waitcnt--;
        pthread_mutex_unlock(&tp->mutex);
    }

    trans_unref(tp, NULL);
    return TRANS_ABORTED;
}


/*
 * Get the current status of a transaction.
 * If the value returned is TRANS_PENDING, then we learn nothing,
 * because unless we are holding the transaction mutex the transaction
 * could be aborted at any time.  However, if the value returned is
 * either TRANS_COMMITTED or TRANS_ABORTED, then that value is the
 * stable final status of the transaction.
 *
 * @param tp    The transaction.
 * @return      The status of the transaction, as it was at the time of call.
 */
TRANS_STATUS trans_get_status(TRANSACTION *tp) {
    return tp->status;
}

/*
 * Print information about a transaction to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 *
 * @param tp    The transaction to be shown.
 */
void trans_show(TRANSACTION *tp) {
    fprintf(stderr, "[id=%d, status=%d, refcnt=%d]", tp->id, tp->status, tp->refcnt);
}

/*
 * Print information about all transactions to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 */
void trans_show_all(void) {
    TRANSACTION *trans = trans_list.next;
    while(trans != &trans_list) {
        trans_show(trans);
        trans = trans->next;
    }
    fprintf(stderr, "\n");
}
