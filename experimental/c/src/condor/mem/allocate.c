#include "allocate.h"

void* Allocate(int size){
	void* ptr = malloc(size);
	if (!ptr) return NULL;
	return ptr;
}

void Free(void* ptr){
	free(ptr);
}