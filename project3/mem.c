#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "mem.h"

int has_initialized = 0;
int m_error;

void *managed_memory_start;
void *last_valid_address;

struct mem_control_block{
	//manage all the memory block
	int is_available;
	//1 for available
	//0 for unavailable
	int size;
};

int mem_init(int size_of_region){
	//initialize memory allocator
	//called one time by a process using our routines
	struct mem_control_block *mcb;
	if(has_initialized == 1 || size_of_region <= 0){//cases where mem_init should return a failure:
	       //mem_init is called more than once; 
	       //size_of_region is less than or equal to 0
		m_error = E_BAD_ARGS;
		return -1;
	}
	int page_size = getpagesize();
        size_of_region = sizeof(mcb) + page_size;
	//for the request of memory in units of the page size later
	int fd = open("/dev/zero", O_RDWR);
	//open the /dev/zero device
	if((managed_memory_start = mmap(NULL, size_of_region, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		perror("mmap");
		exit(1);
	}//map it to the address space of the process
	close(fd);
	//close the device (don't worry, mapping should be unaffected)marks our allocator as being initialized
	mcb = (struct mem_control_block *)managed_memory_start;
	mcb -> is_available = 1;
	mcb -> size = size_of_region;
	last_valid_address = managed_memory_start + size_of_region;
	has_initialized = 1;//marks our allocator as being initialized
	return 0;
}

void *mem_malloc(int size, int style){
	void *current_location;
	struct mem_control_block *current_location_mcb;
	void *memory_location;
	//the location of the memory we will return to
	memory_location = NULL;
	//set memory_location to NULL until we find a suitable location
	current_location = managed_memory_start;
	//begin searching at the start of managed memory
	size = (size + sizeof(struct mem_control_block)) << 3;
	//calculate the actual size of the memory block
	//return 8-byte aligned chunks of memory
	if(style == M_BESTFIT){
		//for the best-fit policy
		//which means searching for the smallest free space that can accommodate the request
		while(current_location != last_valid_address){
			//until we have searched all allocated space
			current_location_mcb = (struct mem_control_block*)current_location;
			//current_location and current_location_mcb point to the same address
			//we use current_location_mcb as a struct and current_location as a void pointer
			//we use current_location to calculate addresses
			if(current_location_mcb -> is_available && current_location_mcb -> size >= size && (memory_location == NULL || current_location_mcb -> size < ((struct mem_control_block *)memory_location) -> size)){
				//we have found the correct location
				current_location_mcb -> is_available = 0;
				memory_location = current_location;
				break;
			}
			current_location = current_location +  current_location_mcb -> size;
			//the current memory block is not suitable, move to the next one
		}
	}
	else if (style == M_WORSTFIT){
		//find the largest chunks
		while(current_location != last_valid_address){
			current_location_mcb = (struct mem_control_block*)current_location;
			if(current_location_mcb -> is_available && current_location_mcb -> size >= size && (memory_location == NULL || current_location_mcb -> size > ((struct mem_control_block *)memory_location) -> size)){
				current_location_mcb -> is_available = 0;
				memory_location = current_location;
				break;
			}
			current_location = current_location +  current_location_mcb -> size;
		}
	}
	else{
		//first-fit only examines free chunks until it finds one that fits
		while(current_location != last_valid_address){
			current_location_mcb = (struct mem_control_block*)current_location;
			if(current_location_mcb -> is_available && current_location_mcb -> size >= size){
				current_location_mcb -> is_available = 0;
				memory_location = current_location;
				break;
			}
			current_location = current_location +  current_location_mcb -> size;
		}
	}
	if(memory_location == NULL){
		//there is not enough contiguous free space within size_of_region
		m_error = E_NO_SPACE;
		return NULL;
	}
	if(!memory_location)
	{//we still don't find the correct location
		//then we need to ask the OS for new memory block
		struct mem_control_block *new_mcb = (struct mem_control_block *)memory_location;
		new_mcb -> is_available = 1;
		int page_size = getpagesize();
		new_mcb -> size = sizeof(new_mcb) + page_size;
		memory_location = last_valid_address;
		//the new memory will be  where the last valid address left off
		last_valid_address = last_valid_address + size;
		//move the last valid address forward size
		current_location_mcb = memory_location;
		current_location_mcb -> is_available = 0;
		current_location_mcb -> size = size;
	}
		memory_location = memory_location + sizeof(struct mem_control_block);
		//move the pointer past the mem_control_block
		return memory_location;
}

void mem_dump(){
	//a debugging routine
	//print the regions of free memory to the screen
	void *current_location;
	current_location = managed_memory_start;
	struct mem_control_block *current_location_mcb;
	while(current_location != last_valid_address){
		current_location_mcb = (struct mem_control_block *)current_location;
		if(current_location_mcb -> is_available){
			printf("%p-%p\n\n", current_location, current_location + current_location_mcb -> size);
		}
		current_location = current_location +  current_location_mcb -> size;
	}
}

int mem_free(void *ptr){
	struct mem_control_block *mcb;
	if(ptr == NULL){
		return -1;
	}
	mcb = ptr - sizeof(mcb);
	//get the first address of the memory control block
	mcb -> is_available = 1;
	//mark the block as being available
	mem_dump();
	return 0;
}
