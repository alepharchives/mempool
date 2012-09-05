#include <stdlib.h>
#include <assert.h>

struct chunk {
	char *freePos;
	struct chunk *next;
};

struct slot {
	struct slot *next;
};
static struct slot *table[8];

static struct chunk *chunkHead;

void* chunkAlloc(int size,char tag) {
	struct chunk *ck;
	void *ret;

	assert(size <= 512);
	ck = chunkHead;
	if(ck->freePos+size+1 > (char*)ck+1024) {
		ck = malloc(1024);
		ck->next = chunkHead;
		ck->freePos = (char*)ck + sizeof(struct chunk);
		chunkHead = ck;
		return chunkAlloc(size,tag);
	}
	ck->freePos[0] = tag;
	ret = ck->freePos + 1;
	ck->freePos += (size+1);
	return ret;	
}

void PoolInit() {
	int i;
	for(i=0; i<8; i++) {
		table[i] = NULL;
	}
	chunkHead = malloc(1024);	//list with head simplify many operations 
	chunkHead->next = NULL;
	chunkHead->freePos = (char*)chunkHead + sizeof(struct chunk);
}

void *PoolAlloc(int size) {
	int i,slot_size;
	void *ret;

	slot_size = 4;
	for(i=0; i<8; i++) {	
		if(slot_size >= size) {
			break;
		}
		slot_size = slot_size << 1;
	}
	if(i < 8) {	//alloc in slot
		if(table[i] != NULL) {
			ret = table[i];
			table[i] = table[i]->next;
			return ret;
		} else {	//alloc from chunk
			return chunkAlloc(slot_size,i);
		}
	} else if(size < 1024-sizeof(struct chunk)-1) {	//alloc a chunk
		struct chunk *ck;
		ck = malloc(1024);
		ck->next = chunkHead->next;
		ck->freePos = (char*)ck + sizeof(struct chunk);
		ck->freePos[0] = 8;	//tag 8 means a whole chunk
		chunkHead->next = ck;

		return ck->freePos + 1;

	} else {	//using malloc directly
		char *ret;
		ret = malloc(size+1);
		ret[0] = 9;	//tag 9 means the memory is malloced
		return ret+1;
	}
}

void PoolFree(void *ptr) {
	int tag;
	char *p;
	p = (char*)ptr-1;
	tag = p[0];	
	assert(tag >=0 && tag <=9);
	switch(tag) {
		case 8:
			//fixme : need change stratage
			break;
		case 9:
			free(p);
			break;
		default:
			((struct slot*)ptr)->next = table[tag];
			table[tag] = ptr;
	}
}

void PoolRelease() {
	struct chunk *p;

	p = chunkHead->next;
	while(p) {
		chunkHead->next = p->next;
		free(p);
		p = chunkHead->next;
	}
	free(chunkHead);
}
