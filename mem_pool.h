// Mark Fletcher
// 2021-04-22
// vim: noet ts=4 sw=4 sts=0

#ifndef MEM_POOL_LOADED
#define MEM_POOL_LOADED 1

#include <stdlib.h>


/* pool_block structure
 *
 * Contains the information related to an individual block of memory for a
 * pool. These blocks can be chained to increase the size of the pool. This
 * structure contains the links to its neighbors (if any) along with other
 * info necessary for a block to be operated on independently.
 */
typedef struct pool_block_s {
	struct pool_block_s * prev;
	struct pool_block_s * next;

	size_t                pool_size;
	size_t                index;

	char                * pool;
} pool_block;


/* pool_info structure
 *
 * Contains the top-level information about a memory pool instance.
 */
typedef struct pool_info_s {
	struct pool_block_s * first;
	struct pool_block_s * current;

	size_t                obj_size;
	size_t                obj_count;

	size_t                block_size;
	size_t                block_count;
} pool_info;


/* Allocate and initialize a memory pool.
 *
 * @param size The size in bytes of the type of data to be stored in the pool
 * @return A pointer to the created pool or NULL on failure
 */
pool_info * pool_init(size_t size);

/* Tear down and deallocate a memory pool.
 * After this, the caller's pool_info pointer will no longer be valid. The
 * caller is advised to avoid attempting to use it any further.
 *
 * @param pool The pool to destroy
 * @return Zero on success, any other value indicates failure
 */
int pool_destroy(pool_info * pool);

/* Push the specified data into the given pool.
 * A return matching the given byte count parameter indicates total success. A
 * return that is less that the given count indicates not all of the data was
 * pushed for whatever reason.
 *
 * @param pool The descriptor for the memory pool to operate on
 * @param data A pointer to the data to be pushed
 * @param bytes The length of the data to be pushed in bytes
 * @return The number of bytes pushed
 */
size_t push_bytes(pool_info * pool, void * data, size_t bytes);

/* Pop the specified amount of bytes off the given pool starting from the
 * current index in the pool.
 *
 * @param pool The descriptor for the memory pool to operate on
 * @param bytes The amount of bytes to pop from the pool
 * @return An array containing the popped data, or NULL on failure
 */
char * pop_bytes(pool_info * pool, size_t bytes);


#endif

