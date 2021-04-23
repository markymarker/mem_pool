// Mark Fletcher
// 2021-04-23
// vim: noet ts=4 sw=4 sts=0

#include <stdio.h>
#include <stdlib.h>

#include "mem_pool.h"

struct test_s {
  int a;
  int b;
  char c;
};

const size_t test_s_size = sizeof(struct test_s);

int main(int argl, char ** argv){
	size_t temps;

	printf("Begin (%lu)\n", sizeof(struct test_s));

	pool_info * pool = pool_init(sizeof(struct test_s));
	if(pool == NULL){
		printf("Failed to initialize\n");
		return 1;
	}
	printf("Initialized\n");

	struct test_s data1 = { 4, 5, 'z' };

	temps = push_bytes(pool, &data1, test_s_size);
	printf("Pushed data\n");
	if(temps != test_s_size){
		printf("Data pushed not equal: e(%lu) r(%lu)\n", test_s_size, temps);
	}

	printf(pool_destroy(pool) ? "Error on destroying\n" : "Destroyed\n");
}

