#include <stdio.h>
#include "testlist.h"
#define THREAD_COUNT 8  //线程数
struct timeval start;
struct timeval end;
list_t  aList;
//gettimeofday($start, NULL);

double gettimeinterval(struct timeval start, struct timeval end)
{
	return 1000000*(end.tv_sec-start.tv_sec) + end.tv_usec - start.tv_usec;	
}

int main()
{
	
	list_init(&aList);
	long thread;
	//int i;
	pthread_t* thread_handles;
	int thread_count = THREAD_COUNT;
	thread_handles = malloc(thread_count*sizeof(pthread_t)); 
	for(thread=0;thread<thread_count;thread++)
			pthread_create(&thread_handles[thread],NULL, function, (void*)thread);
	for(thread=0; thread<thread_count; thread++)
		pthread_join(thread_handles[thread],NULL);

//	printf("%d \n", counter_get_value(&counter));
	return 0;
}

void *function (void *rank)
{
	//long my_rank = (long)rank;
	int i ;	
        //list_t t1;
//	list_init(aList);
//	for(i = 0 ;i<1000;i++)
	list_insert(&aList,15);
	printlist(&aList);
	return NULL;
}
