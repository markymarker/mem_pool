// Mark Fletcher
// 2021-04-22
// vim: noet ts=4 sw=4 sts=0

#include "mem_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// The approximate byte count to shoot for when determining pool block size
#define TARGET_BLOCK_SIZE 2048


/* Internal function to allocate and initialize a new memory pool block for the
 * given memory pool. Information from the given memory pool will be used to
 * set values for the created block as appropriate, and the given memory pool
 * will be updated as appropriate.
 *
 * @param pool The pool to add a block to
 * @return The created block, or NULL on failure
 */
pool_block * pool_add_block(pool_info * pool){
	pool_block * pb = (pool_block *)malloc(sizeof(pool_block));
	if(pb == NULL){
		fprintf(stderr, "Unable to allocate pool block!\n");
		// TODO: Print error info
		return pb;
	}

	// Set up new block

	pb->prev = pool->current;
	pb->next = NULL;

	pb->pool_size = pool->obj_size * pool->obj_count;
	pb->index = 0;

	pb->pool = (char *)malloc(pb->pool_size);
	if(pb->pool == NULL){
		fprintf(stderr, "Unable to allocate pool of (%lu) bytes!", pb->pool_size);
		// TODO: Print error info
		free(pb);
		return NULL;
	}

	// Update pool

	if(pool->current != NULL) pool->current->next = pb;
	pool->current = pb;
	++pool->block_count;

	return pb;
}


/* Internal function to remove the topmost block from the given memory pool.
 * Performs cleanup necessary inside of block, then cleans up block and updates
 * values in the pool itself.
 *
 * @param pool The pool to remove a block from
 * @return Zero on success, any other value indicates failure
 */
int pool_free_block(pool_info * pool){
	pool_block * to_free = pool->current;
	if(to_free == NULL) return 0;

	// Normally, the index of the block should be rewound, indicating the data
	// has been fully consumed. Seeing when data is left unconsumed may be
	// useful in some debugging circumstances.
	if(to_free->index != 0){
		fprintf(stderr, "DEBUG: Freeing block with advanced index\n");
	}

	pool->current = to_free->prev;
	--pool->block_count;

	// Remove soon-to-be-dangling pointer from new current block.
	// If the block removed was the pool's first, update pool info accordingly.
	if(pool->current != NULL){
		pool->current->next = NULL;
	} else {
		pool->first = NULL;
	}

	free(to_free->pool);
	free(to_free);
	return 0;
}


/* Try to come up with a reasonable size for each pool block to be, based on
 * the size of the objects that will be stored.
 *
 * @param obj_size The size of the objects to be stored
 */
size_t figure_block_size(size_t obj_size){
	if(obj_size >= (TARGET_BLOCK_SIZE / 2)) return 2;
	return TARGET_BLOCK_SIZE / obj_size;
}


/* Implementation of pool_init from header file.
 */
pool_info * pool_init(size_t size){
	pool_info * pi = (pool_info *)malloc(sizeof(pool_info));
	if(pi == NULL){
		fprintf(stderr, "Unable to allocate memory pool!");
		// TODO: Print error info
		return pi;
	}

	pi->obj_size = size;
	pi->obj_count = figure_block_size(size);

	pi->block_size = pi->obj_size * pi->obj_count;
	pi->block_count = 0;

	pi->first = pi->current = NULL;
	pool_add_block(pi);
	pi->first = pi->current;

	return pi;
}


/* Implementation of pool_destroy from header file.
 */
int pool_destroy(pool_info * pool){
	int ret = 0;

	for(pool_block * pb = pool->first; pb != NULL; ){
		pool_block * next = pb->next;
		free(pb->pool);
		free(pb);
		pb = next;
	}

	free(pool);

	return ret;
}


/* Implementation of push_bytes from header file.
 */
size_t push_bytes(pool_info * pool, void * data, size_t bytes){
	if(bytes == 0) return 0;

	// Attempt to add a block if none are present
	if(!pool->current && !pool_add_block(pool)) return 0;

	size_t push_total = 0;
	size_t blocks_count = 1;
	size_t max_blocks_expect = 1 + (bytes / pool->block_size);
	pool_block * target = pool->current;

	// Copy as much as we can to the current block, then add a new block and
	// rinse and repeat until all is copied or we get bored or both.
	while(push_total < bytes){
		if(target == NULL) return push_total;

		// Determine how much we have room for
		size_t push_piece = target->pool_size - target->index;
		if(push_piece > bytes) push_piece = bytes;

		// Determination will have given 0 if the current block is full, so
		// boss says add a new one.
		if(push_piece == 0){
			target = pool_add_block(pool);
			++blocks_count;
			continue;
		}

		// Finally, the main event
		memcpy(target->pool + target->index, (char *)data + push_total, push_piece);
		push_total += push_piece;

		// Sanity check -- keeps the doctor away
		if(blocks_count > max_blocks_expect){
			fprintf(stderr,
				"Blocks exceeded expected for push (%lu) (%lu)\n",
				blocks_count,
				max_blocks_expect
			);
			break;
		}
	}

	return push_total;
}


/* Implementation of pop_bytes from header file.
 * TODO
 */
size_t pop_bytes(pool_info * pool, char * dest, size_t bytes){
	return 0;
}

