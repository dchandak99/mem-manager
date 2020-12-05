#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "alloc.h"

struct chunk
{
   size_t size;
   //struct chunk *next;
   int is_free;
};

struct chunk chunks[512];

int N = 1024;	// 1k*4B = 4kB
char* start_addr;

int init_alloc()
{	
	for(int i = 0; i < 512; ++i)
		chunks[i].is_free = 1;			// == 1 means it is free

	start_addr = mmap(NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if(start_addr == MAP_FAILED){
        //printf("Mapping Failed\n");
        return 1;
    }


    return 0;
}

int cleanup()
{
	return munmap(start_addr, N*sizeof(int));
}

char *alloc(int buf_size)
{
	if(buf_size%8 != 0 || buf_size==0)
		return NULL;

	//check if insuff. free space then return NULL
	bool alloced = 0;
	int num_blocks = buf_size/8;

	for(int i = 0; i < 512; ++i)
	{
		if(chunks[i].is_free == 1)
		{	
			bool can = 1;
			for(int j = 1; j < num_blocks; ++j)
			{
				if((i+j)>511 || chunks[i+j].is_free == 0)
				{
					can = 0;
					break;
				}
			}
			if(can == 1)
			{
				alloced = 1;
				for(int j = 0; j < num_blocks; ++j)
				{
					chunks[i+j].is_free = 0;
					chunks[i+j].size = -1;
				}
				chunks[i].size = buf_size;
				//char *ra = start_addr;
				//return ra + i*8;
				return start_addr + i*8;
			}
		}
	}

	if(alloced == 0)
		return NULL;

}

void dealloc(char* addr)
{
	int id = (addr - start_addr)/8;

	int num_blocks = chunks[id].size/8;

	for(int i = 0; i < num_blocks; ++i)
	{
		chunks[id + i].is_free = 1;
	}

}


