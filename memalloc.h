#include <stdlib.h>

#define TRUE		1;
#define FALSE		0;

typedef char ALIGN[16];
//TODO: make a testing directory
union header{ //TODO: use proper software engineering and split up the data and the node
    struct { //TODO: is it ok to use an unamed struct
	size_t size;
	unsigned isFree;
	union header *next;
    } s;
    ALIGN stub;
};

typedef union header headerT;

void *memAlloc(size_t);

void free(void*);
