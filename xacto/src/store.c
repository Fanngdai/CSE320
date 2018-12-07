// #include "store.h"
// #include "csapp.h"
// #include "debug.h"

/*
 * Initialize the store.
 */
// void store_init(void) {

// }

/*
 * Finalize the store.
 */
// void store_fini(void) {

// }

/*
 * Put a key/value mapping in the store.  The key must not be NULL.
 * The value may be NULL, in which case this operation amounts to
 * deleting any existing mapping for the given key.
 *
 * This operation inherits the key and consumes one reference on
 * the value.
 *
 * @param tp  The transaction in which the operation is being performed.
 * @param key  The key.
 * @param value  The value.
 * @return  Updated status of the transation, either TRANS_PENDING,
 *   or TRANS_ABORTED.  The purpose is to be able to avoid doing further
 *   operations in an already aborted transaction.
 */
// TRANS_STATUS store_put(TRANSACTION *tp, KEY *key, BLOB *value) {

// }

/*
 * Get the value associated with a specified key.  A pointer to the
 * associated value is stored in the specified variable.
 *
 * This operation inherits the key.  The caller is responsible for
 * one reference on any returned value.
 *
 * @param tp  The transaction in which the operation is being performed.
 * @param key  The key.
 * @param valuep  A variable into which a returned value pointer may be
 *   stored.  The value pointer store may be NULL, indicating that there
 *   is no value currently associated in the store with the specified key.
 * @return  Updated status of the transation, either TRANS_PENDING,
 *   or TRANS_ABORTED.  The purpose is to be able to avoid doing further
 *   operations in an already aborted transaction.
 */
// TRANS_STATUS store_get(TRANSACTION *tp, KEY *key, BLOB **valuep) {

// }

/*
 * Print the contents of the store to stderr.
 * No locking is performed, so this is not thread-safe.
 * This should only be used for debugging.
 */
// void store_show(void) {

// }

