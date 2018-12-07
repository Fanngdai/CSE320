#include "transaction.h"
#include "csapp.h"
#include "debug.h"

TRANSACTION trans_list;

/*
 * Initialize the transaction manager.
 */
void trans_init(void) {
    debug("Initialize transaction manager");
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
    debug("Create new transaction 0");
    TRANSACTION *tp = Calloc(sizeof(TRANSACTION), sizeof(char));

    if(pthread_mutex_init(&tp->mutex, NULL) < 0) {
        if(tp) Free(tp);
        return NULL;
    }

    tp->refcnt = 1;


    // bp->content = Calloc(size, sizeof(char));
    // memcpy(bp->content, content, size);
    // bp->prefix = bp->content;
    // bp->size = size;

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
    if(!tp || pthread_mutex_lock(&trans_list.mutex) < 0) return NULL;
    debug("Increase ref count on transaction %d (%d -> %d) for newly created transaction", tp->id, tp->refcnt, tp->refcnt + 1);
    debug("Increase ref count on transaction %d (%d -> %d) (null)", tp->id, tp->refcnt, tp->refcnt + 1);
    tp->refcnt++;
    if(pthread_mutex_unlock(&trans_list.mutex) < 0) return NULL;
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
    debug("Decrease ref count on transaction 0 (3 -> 2) (null)");
    debug("Decrease ref count on transaction 0 (3 -> 2) for attempting to commit transaction");
    if(!tp || pthread_mutex_lock(&trans_list.mutex) < 0) return;
    trans_list.refcnt--;
    if(tp->refcnt == 0) {
        Free(tp);
        tp = NULL;
    }
    if(pthread_mutex_unlock(&trans_list.mutex) < 0) return;
}

/*
 * Add a transaction to the dependency set for this transaction.
 *
 * A transaction may become "dependent" on another transaction.
 * This occurs when a transaction accesses an entry in the store that has
 * previously been accessed by a pending transaction having a smaller transaction
 * ID.  Once that occurs, the dependent transaction cannot commit until the
 * the transaction on which it depends has committed.  Moreover, if the
 * other transaction aborts, then the dependent transaction must also abort.
 *
 * The dependencies of a transaction are recorded in a "dependency set",
 * which is part of the representation of a transaction.  A dependency set
 * is represented as a singly linked list of "dependency" nodes having the
 * following structure.  Any given transaction can occur at most once in a
 * single dependency set.
 *
 * @param tp                The transaction to which the dependency is being added.
 * @param dtp               The transaction that is being added to the dependency set.
 */
void trans_add_dependency(TRANSACTION *tp, TRANSACTION *dtp) {
    debug("Release 0 waiters dependent on transaction 0");
// typedef struct dependency {
//   struct transaction *trans;  // Transaction on which the dependency depends.
//   struct dependency *next;    // Next dependency in the set.
// } DEPENDENCY;
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
 * @param tp        The transaction to be committed.
 * @return          The final status of the transaction: either TRANS_ABORTED,
 *                  or TRANS_COMMITTED.
 */
TRANS_STATUS trans_commit(TRANSACTION *tp) {
    if(!tp || pthread_mutex_lock(&trans_list.mutex) < 0) return TRANS_ABORTED;
    // must call sem wait on all transactions in its dependency set
    // use waitcnt field to see how many is waiting
    debug("Transaction 0 trying to commit");
    debug("Transaction 0 commits");
    trans_unref(tp, NULL);
    Free(tp);
    tp = NULL;
    // calls release dependent and trans unref
    if(pthread_mutex_unlock(&trans_list.mutex) < 0) return TRANS_ABORTED;
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
 * @param tp        The transaction to be aborted.
 * @return          TRANS_ABORTED.
 */
TRANS_STATUS trans_abort(TRANSACTION *tp) {
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
 * @param tp        The transaction.
 * @return          The status of the transaction, as it was at the time of call.
 */
TRANS_STATUS trans_get_status(TRANSACTION *tp) {
    return tp->status;
}

/*
 * Print information about a transaction to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 *
 * @param tp        The transaction to be shown.
 */
void trans_show(TRANSACTION *tp) {

}

/*
 * Print information about all transactions to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 */
void trans_show_all(void) {

}
