#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "mem.h"

int has_initialized = 0;
int m_error;

void *managed_memory_start;
void *last_valid_address;

struct mem_control_block
{
    //manage all the memory block
    int is_available;
    //1 for available
    //0 for unavailable
    int size;
};

int mem_init(int size_of_region)
{
    //initialize memory allocator
    //called one time by a process using our routines
    struct mem_control_block *mcb;
    if(has_initialized == 1 || size_of_region <= 0) //cases where mem_init should return a failure:
    {
        //mem_init is called more than once;
        //size_of_region is less than or equal to 0
        m_error = E_BAD_ARGS;
        return -1;
    }
    int page_size = getpagesize();
    size_of_region = ((sizeof(mcb) + size_of_region-1)/page_size+1)*page_size;//取需要的最小整页面大小
    //for the request of memory in units of the page size later
    int fd = open("/dev/zero", O_RDWR);
    //open the /dev/zero device
    if((managed_memory_start = mmap(NULL, size_of_region, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        m_error = E_BAD_ARGS;
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

void *mem_alloc(int size, int style)
{
    void *current_location;
    struct mem_control_block *current_location_mcb;
    void *memory_location;
    //the location of the memory we will return to
    memory_location = NULL;
    //set memory_location to NULL until we find a suitable location
    current_location = managed_memory_start;
    //begin searching at the start of managed memory
    size =(((size + sizeof(struct mem_control_block)-1)/8) << 3)+8;//计算最小所需8-byte size
    //calculate the actual size of the memory block
    //return 8-byte aligned chunks of memory
    if(style == M_BESTFIT)
    {
        //for the best-fit policy
        //which means searching for the smallest free space that can accommodate the request
        while(current_location != last_valid_address)
        {
            //until we have searched all allocated space
            current_location_mcb = (struct mem_control_block*)current_location;
            //current_location and current_location_mcb point to the same address
            //we use current_location_mcb as a struct and current_location as a void pointer
            //we use current_location to calculate addresses
            if(current_location_mcb -> is_available && current_location_mcb -> size >= size && (memory_location == NULL || current_location_mcb -> size < ((struct mem_control_block *)memory_location) -> size))
            {
                //we have found the correct location
                current_location_mcb -> is_available = 0;
                memory_location = current_location;
                break;
            }
            current_location = current_location +  current_location_mcb -> size;
            //the current memory block is not suitable, move to the next one
        }
    }
    else if (style == M_WORSTFIT)
    {
        //find the largest chunks
        while(current_location != last_valid_address)
        {
            current_location_mcb = (struct mem_control_block*)current_location;
            if(current_location_mcb -> is_available && current_location_mcb -> size >= size && (memory_location == NULL || current_location_mcb -> size > ((struct mem_control_block *)memory_location) -> size))
            {
                current_location_mcb -> is_available = 0;
                memory_location = current_location;
                break;
            }
            current_location = current_location +  current_location_mcb -> size;
        }
    }
    else if	(style == M_FIRSTFIT)
    {
        //first-fit only examines free chunks until it finds one that fits
        while(current_location != last_valid_address)
        {
            current_location_mcb = (struct mem_control_block*)current_location;
            if(current_location_mcb -> is_available && current_location_mcb -> size >= size)
            {
                current_location_mcb -> is_available = 0;
                memory_location = current_location;
                break;
            }
            current_location = current_location +  current_location_mcb -> size;
        }
    }
	else
	{
		m_error = E_NO_SPACE;
        return NULL;
	}
    if(memory_location == NULL)
    {
        //there is not enough contiguous free space within size_of_region
        m_error = E_NO_SPACE;
        return NULL;
    }

   	struct mem_control_block *next_mcb=current_location+size;
	if(next_mcb!=current_location+current_location_mcb->size)
	{
		next_mcb->is_available=1;
		next_mcb->size=current_location_mcb->size-size;
	}
	current_location_mcb->size=size;
    memory_location = memory_location + sizeof(struct mem_control_block);
    //move the pointer past the mem_control_block
    return memory_location;
}

void mem_dump()
{
    //a debugging routine
    //print the regions of free memory to the screen
    void *current_location;
    current_location = managed_memory_start;
    struct mem_control_block *current_location_mcb;
    while(current_location != last_valid_address)
    {
        current_location_mcb = (struct mem_control_block *)current_location;
        if(current_location_mcb -> is_available)
        {
            printf("%p-%p\n\n", current_location, current_location + current_location_mcb -> size);
			//printf("in dump\n");
        }
        current_location = current_location +  current_location_mcb -> size;
    }
}

int mem_free(void *ptr)
{
    void* location=ptr-8;
    struct mem_control_block *mcb = ptr-8;
    if(ptr == NULL)
    {
        return -1;
    }
	mcb -> is_available = 1;
    //merger if the block after this block is avaliable
    struct mem_control_block *next_mcb = location+mcb->size;
    if(next_mcb->is_available)
        mcb->size+=next_mcb->size;



    void *previous = managed_memory_start;
    struct mem_control_block *premcb;
    while(previous<ptr)
    {
        premcb = (struct mem_control_block*)previous;
        if(premcb->is_available==1&&previous+premcb->size==ptr-8)
        {
            premcb->size=premcb->size+mcb->size;
            break;
        }
        previous+=(premcb->size);
    }
    mem_dump();
    return 0;
}
int main()
{
    char *p1,*p2,*p3,*p4;
    mem_init(4096);
    mem_dump();
    p1 =(char*) mem_alloc(20*sizeof(char),0);
    strcpy(p1,"test mem_alloc");
    puts(p1);
	mem_dump();

	p2 =(char*) mem_alloc(20*sizeof(char),0);
	mem_dump();
	p3 =(char*) mem_alloc(20*sizeof(char),0);
	mem_dump();
    mem_free(p2);
    p4 =(char*) mem_alloc(16*sizeof(char),0);
    mem_dump();
    mem_free(p1);
    mem_free(p4);
    mem_free(p3);


    return 0;
}
