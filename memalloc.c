#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "memalloc.h"

headerT *getFreeBlock(size_t);
//TODO: combine free blocks together if they are beside each other
headerT *head, *tail;
pthread_mutex_t globalMallocLock;

//TODO: for multi threading can I make a threaded function that runs until i call a join function and have it just end once I do that (have it check a variable that i can control from the main thread?)

void *memAlloc(size_t size){ // this code below is coppied from the tutorial //TODO: change to use mmap because sbrk is linix specific
    size_t totalSize;
    void *block;
    headerT *header; //TODO: why are we using lock and unlock instead of wait and post
    if(!size)
	return NULL;
    pthread_mutex_lock(&globalMallocLock);
    header = getFreeBlock(size); // this getFreeBlock loops through our linked list and checks if there is an already free block of memory that works for our needed size

    if(header){
	header->s.isFree = FALSE;
	pthread_mutex_unlock(&globalMallocLock);
	return (void*)(header+1); // look to the last return statment to see an explination
    }
    totalSize = sizeof(headerT) + size;
    block = sbrk(totalSize);
    
    if(block == (void*)-1){
	pthread_mutex_unlock(&globalMallocLock);
	return NULL;
    } // here and up ^^ checks to see if there is a free block of memory already allocated that can be used
    header = block;
    header->s.size = size;
    header->s.isFree = FALSE;
    header->s.next = NULL;
    
    if(!head)
	head = header;
    if(tail) // why are we not just setting the tail to point to it
	tail->s.next = header;

    tail = header;
    pthread_mutex_unlock(&globalMallocLock);

    return (void*)(header+1); // the header + 1 points to the byte right after the end of the header which is also the first byte of the memory block we allocated for the user
}


headerT *getFreeBlock(size_t size){// have this function or another one carve up memory if it's a block that is bigger then what was asked for
    headerT *curr = head;
    while(curr){
	if(curr->s.isFree && curr->s.size >= size)
	    return curr;
	curr = curr->s.next;
    }
    return NULL;
}


void free(void *block){ //TODO: have free or some other function combine slots of memory that are beside each other
    headerT *header, *temp;
    void *programbreak;

    if(!block)
	return;
    pthread_mutex_lock(&globalMallocLock);
    // cast the bock to a header type and move the address back by 1 unit
    header = (headerT*)block -1; // equivalent ((headerT*)block)-1;
    
    programbreak = sbrk(0); // sbrk(0) returns pointer to the end of the heap so we check if at the end of the current block is equal to it
    if((char*)block + header->s.size == programbreak){ //TODO: in testing check if casting it to a char is manditory
	if(head == tail){
	    head = tail = NULL;
	} else{
	    temp = head;
	    while(temp){
		if(temp->s.next == tail){
		    temp->s.next = NULL;
		    tail = temp;
		}
		temp = temp->s.next;
	    }
	}

	sbrk(0-sizeof(headerT)-header->s.size);
	pthread_mutex_unlock(&globalMallocLock);
	return;
    }

    header->s.isFree = TRUE;
    pthread_mutex_unlock(&globalMallocLock);


}

