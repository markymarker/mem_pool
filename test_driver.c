// Mark Fletcher
// 2021-04-23
// vim: noet ts=4 sw=4 sts=0

#include <stdio.h>
#include <stdint.h>

#include "mem_pool.h"

// Struct includes a mix of things to also look at packing and alignment when
// inspecting the pool. You know, for science and stuff.
struct test_s {
  uint64_t a;
  uint32_t b;
  unsigned char c;
  uint16_t d;
};

const size_t test_s_size = sizeof(struct test_s);


void dump_pool(pool_info * pool){
	const char * delim = "\n\n\0\0\0\0\0\0\0\0\n\n";
	printf("Dump pool::");
	fwrite(delim, 1, 12, stdout);
	fwrite(pool->current->pool, 1, pool->current->pool_size, stdout);
	fwrite(delim, 1, 12, stdout);
}


int main(int argl, char ** argv){
	size_t temps;

	printf("Begin (%lu)\n", sizeof(struct test_s));

	pool_info * pool = pool_init(sizeof(struct test_s));
	if(pool == NULL){
		printf("Failed to initialize\n");
		return 1;
	}
	printf("Initialized\n");

	size_t dsize = 4 * test_s_size;
	struct test_s datas[] = {
		{ 0x1111222233334444, 0xAAAABBBB, 0xCC, 0xDD},
		{ -1, -2, 0x66, -1 },
		{ 4, 5, 'a', 'z' },
		{ -1, -1, -1, -1 },
	};

	temps = push_bytes(pool, datas, dsize);
	printf("Pushed data\n");
	if(temps != dsize){
		printf("Data pushed not equal: e(%lu) r(%lu)\n", dsize, temps);
	}

	dump_pool(pool);

	printf(pool_destroy(pool) ? "Error on destroying\n" : "Destroyed\n");
}

