#include "data.h"
#include "csapp.h"
#include "debug.h"

#include "store.h"

#include <string.h>

/*
 * Create a blob with given content and size.
 * The content is copied, rather than shared with the caller.
 * The returned blob has one reference, which becomes the caller's
 * responsibility.
 *
 * @param content   The content of the blob.
 * @param size      The size in bytes of the content.
 *
 * @return          The new blob, which has reference count 1.
 */
BLOB *blob_create(char *content, size_t size) {
    if(size < 0) return NULL;
    BLOB *bp = Calloc(sizeof(BLOB), sizeof(char));

    if(!bp) {
        return NULL;
    } else if(pthread_mutex_init(&bp->mutex, NULL) < 0) {
        Free(bp);
        bp = NULL;
        return NULL;
    }

    bp->refcnt = 1;
    if(content) {
        bp->content = Calloc(size+1, sizeof(char));
        memcpy(bp->content, content, size);
        bp->prefix = bp->content;
        if(bp->content) debug("Create blob with content %p, size %lu -> %p", bp->content, size, bp);
    }
    bp->size = size;

    return bp;
}

/*
 * Increase the reference count on a blob.
 *
 * @param bp        The blob.
 * @param why       Short phrase explaining the purpose of the increase.
 *
 * @return          The blob pointer passed as the argument.
 */
BLOB *blob_ref(BLOB *bp, char *why) {
    if(!bp || pthread_mutex_lock(&bp->mutex) < 0) return NULL;

    bp->refcnt++;
    if(bp->content) debug("Increase reference count on blob %p [%s] (%d -> %d) %s", bp, bp->content, bp->refcnt, bp->refcnt, why);

    if(pthread_mutex_unlock(&bp->mutex) < 0) return NULL;
    return bp;
}

/*
 * Decrease the reference count on a blob.
 * If the reference count reaches zero, the blob is freed.
 *
 * @param bp        The blob.
 * @param why       Short phrase explaining the purpose of the decrease.
 */
void blob_unref(BLOB *bp, char *why) {
    if(!bp || pthread_mutex_lock(&bp->mutex) < 0) return;

    bp->refcnt--;
    if(bp->content) debug("Decrease reference count on blob %p [%s] (%d -> %d) %s", bp, bp->content, bp->refcnt+1, bp->refcnt, why);
    if(bp->refcnt == 0) {
        if(bp->content) {
            debug("Free blob %p [%s]", bp, bp->content);
            Free(bp->content);
            bp->content = NULL;
        }
        Free(bp);
        bp=NULL;
    } else {
        if(pthread_mutex_unlock(&bp->mutex) < 0) return;
    }
}

/*
 * Compare two blobs for equality of their content.
 *
 * @param bp1   The first blob.
 * @param bp2   The second blob.
 *
 * @return      0 if the blobs have equal content, nonzero otherwise.
 */
int blob_compare(BLOB *bp1, BLOB *bp2) {
    if(pthread_mutex_lock(&bp1->mutex) < 0 || pthread_mutex_lock(&bp2->mutex) < 0)
        return -1;

    int max = bp1->size > bp2->size? bp1->size : bp2->size;
    int rtn = strncmp(bp1->content, bp2->content, max);

    if(pthread_mutex_unlock(&bp1->mutex) < 0 || pthread_mutex_unlock(&bp2->mutex) < 0)
        return -1;

    return rtn;
}

/*
 * Hash function for hashing the content of a blob.
 *
 * @param bp    The blob.
 *
 * @return      Hash of the blob.
 */
int blob_hash(BLOB *bp) {
    if(!bp) return -1;
    return bp->content ? *(bp->content)%NUM_BUCKETS : 0;
    // int rtn = 0;
    // for(int i=0; i<bp->size; i++) {
    //     rtn += *bp->content++;
    // }
    // return rtn%NUM_BUCKETS;
}

/*
 * Create a key from a blob.
 * The key inherits the caller's reference to the blob.
 *
 * @param bp    The blob.
 * @return      The newly created key.
 */
KEY *key_create(BLOB *bp) {
    if(!bp) return NULL;
    KEY *kp = Calloc(sizeof(KEY), sizeof(char));

    kp->hash = blob_hash(bp);
    kp->blob = bp;
    // kp->blob = blob_ref(bp, NULL);

    debug("Create key from blob %p -> %p", bp, kp);
    return kp;
}

/*
 * Dispose of a key, decreasing the reference count of the contained blob.
 * A key must be disposed of only once and must not be referred to again
 * after it has been disposed.
 *
 * @param kp    The key.
 */
void key_dispose(KEY *kp) {
    if(!kp) return;
    if(kp->blob->content) debug("Dispose of key %p [%s]", kp, kp->blob->content);

    blob_unref(kp->blob, NULL);

    Free(kp);
    kp = NULL;
}

/*
 * This is not used bc I was told that in key_compare, you compare the content only & not everything
 */
int blob_compare_everything(BLOB *bp1, BLOB *bp2) {
    if(pthread_mutex_lock(&bp1->mutex) < 0 || pthread_mutex_lock(&bp2->mutex) < 0)
        return -1;

    int rtn = 0;

    if(bp1->refcnt != bp1->refcnt)
        rtn = abs(bp1->refcnt-bp2->refcnt);
    if(!rtn && bp1->size != bp2->size)
        rtn = abs(bp1->size-bp2->size);
    if(!rtn) {
        int max = bp1->size > bp2->size? bp1->size : bp2->size;
        rtn = strncmp(bp1->content, bp2->content, max);
    }

    if(pthread_mutex_unlock(&bp1->mutex) < 0 || pthread_mutex_unlock(&bp2->mutex) < 0)
        return -1;

    return rtn;
}

/*
 * Compare two keys for equality.
 *
 * @param kp1       The first key.
 * @param kp2       The second key.
 *
 * @return          0 if the keys are equal, otherwise nonzero.
 */
int key_compare(KEY *kp1, KEY *kp2) {
    return kp1->hash == kp2->hash ? blob_compare(kp1->blob, kp2->blob) : -1;
}


/*
 * Create a version of a blob for a specified creator transaction.
 * The version inherits the caller's reference to the blob.
 * The reference count of the creator transaction is increased to
 * account for the reference that is stored in the version.
 *
 * @param tp        The creator transaction.
 * @param bp        The blob.
 *
 * @return          The newly created version.
 */
VERSION *version_create(TRANSACTION *tp, BLOB *bp) {
    if(!tp) tp = trans_create();

    if(bp && bp->content) {
        debug("Create version of blob %p [%s] for transaction %d -> %p", bp, bp->content, tp->id, tp );
    } else if(!bp) {
        debug("Create NULL version for transaction %d -> %p", tp->id, tp );
    }

    VERSION *vp = Calloc(sizeof(VERSION), sizeof(char));

    vp->creator = trans_ref(tp, NULL);
    // vp->blob = blob_ref(bp, NULL);
    vp->blob = bp;

    return vp;
}

/*
 * Dispose of a version, decreasing the reference count of the
 * creator transaction and contained blob.  A version must be
 * disposed of only once and must not be referred to again once
 * it has been disposed.
 *
 * @param vp        The version to be disposed.
 */
void version_dispose(VERSION *vp) {
    if(!vp) return;
     debug("Dispose of version %p", vp);

    trans_unref(vp->creator, NULL);
    blob_unref(vp->blob, NULL);
    Free(vp);
    vp = NULL;
}
