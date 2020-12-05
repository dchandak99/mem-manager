#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ealloc.h"

struct chunk
{
   size_t size;
   //struct chunk *next;
   int is_free;
};

struct chunk chunks[4][16];	// page 1	== 16*256 slots in 1 page
/*struct chunk chunks2[16];	// page 2	
struct chunk chunks3[16];	// page 3	
struct chunk chunks4[16];	// page 4	
*/

int N = 1024;	// 1k*4B = 4kB
char* start_addr[4]; //char* start_addr2; char* start_addr3; char* start_addr4;

void init_alloc()
{	
	for(int i = 0; i < 16; ++i)
	{
		chunks[0][i].is_free = 1;			// == 1 means it is free
		chunks[1][i].is_free = 1;
		chunks[2][i].is_free = 1;			
		chunks[3][i].is_free = 1;
	}	

	//start_addr = mmap(NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	//start_addr1 = start_addr2 = start_addr3 = start_addr4 = NULL;
	for(int i = 0; i < 4; ++i)
	{
		start_addr[i] = NULL;
	}	

}

void cleanup()
{
	return;
}

char* alloc_helper(int page_id, int buf_size)	// takes page number & buf_size and tries to fit;; NULL if not fit
{
	if(buf_size%256 != 0 || buf_size>4096 || buf_size==0)
		return NULL;

	//check if insuff. free space then return NULL
	if(start_addr[page_id] == NULL)
	{
		start_addr[page_id] = mmap(NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	}

	/*if(start_addr[page_id] == MAP_FAILED){
        //printf("Mapping Failed\n");
        return NULL;
    }*/

	bool alloced = 0;
	int num_blocks = buf_size/256;

	for(int i = 0; i < 16; ++i)
	{
		if(chunks[page_id][i].is_free == 1)
		{	
			bool can = 1;
			for(int j = 1; j < num_blocks; ++j)
			{
				if((i+j)>15 || chunks[page_id][i+j].is_free == 0)
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
					chunks[page_id][i+j].is_free = 0;
					chunks[page_id][i+j].size = -1;
				}
				chunks[page_id][i].size = buf_size;
				//char *ra = start_addr;
				//return ra + i*8;
				return start_addr[page_id] + i*256;
			}
		}
	}

	if(alloced == 0)
		return NULL;
}

char *alloc(int buf_size)	// seg fault in alloc
{
	for(int i = 0; i < 4; ++i)
	{	
		//return alloc_helper(i, buf_size);
		char* x = alloc_helper(i, buf_size);
		if(x != NULL)
			return x;
	}	

	return NULL;

}

bool dealloc_helper(int page_id, char* addr)
{	
	int id = (addr - start_addr[page_id])/256;
	if(id<0 || id>255)
		return false;


	int num_blocks = chunks[page_id][id].size/256;

	for(int i = 0; i < num_blocks; ++i)
	{
		chunks[page_id][id + i].is_free = 1;
	}

	return true;
}

void dealloc(char* addr)
{
	for(int i = 0; i < 4; ++i)
	{
		if(dealloc_helper(i, addr))
			return;
	}	

	return;
}


