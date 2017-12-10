#include <stdio.h>
#include "test_counter.h"

#define MAX 100000 //each thread would call counter_increment(&counter) in a loop max times
struct timeval start;
struct timeval end;
counter_t counter;
//gettimeofday(&start, NULL);

long int get_time_interval(struct timeval start, struct timeval end)
{
	return 1000000*(end.tv_sec-start.tv_sec) + end.tv_usec - start.tv_usec;	
}

int main()
{
	
	long thread;
	int i;
	pthread_t* thread_handles;
	int thread_count;
	for(i=0;i<10;i++)
	{	
		printf("------------------------test%d----------------------\n", i );
		for(thread_count=1;thread_count<=20;thread_count++)
		{	
			counter_init(&counter, 0);
			gettimeofday(&start, NULL);
			thread_handles = malloc(thread_count*sizeof(pthread_t)); 
			for(thread=0;thread<thread_count;thread++)
				pthread_create(&thread_handles[thread],NULL, function, (void*)thread);
			for(thread=0; thread<thread_count; thread++)
				pthread_join(thread_handles[thread],NULL);
			gettimeofday(&end, NULL);
			//printf("lock_type: %d, thread_count: %d, time: %ld us \n",LOCK_TYPE, thread_count, get_time_interval(start,end));
			printf("%ld, %d\n", get_time_interval(start,end), counter_get_value(&counter));
		}
		
	}	
	return 0;
}

void *function (void *rank)
{
	long my_rank = (long)rank;
	int i;
	for(i=0;i<MAX;i++)
	{
		counter_increment(&counter, my_rank);
	}
	return NULL;
}
