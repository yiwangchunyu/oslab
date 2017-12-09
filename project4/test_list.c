#include <stdio.h>
#include "testlist.h"
#define MAX 100000
#define THREAD_COUNT 8  //线程数
struct timeval start;
struct timeval end;
list_t  aList;
//gettimeofday($start, NULL);

long int get_time_interval(struct timeval start, struct timeval end)
{

	return 1000000*(end.tv_sec-start.tv_sec) + end.tv_usec - start.tv_usec;	

}

int main()
{
	
	list_init(&aList);
	long thread;
	//int i;
	pthread_t* thread_handles;
	int thread_count=1;
	for(thread_count=1;thread_count<=20;thread_count++)
	{
	gettimeofday(&start, NULL);
	thread_handles = malloc(thread_count*sizeof(pthread_t)); 
	for(thread=0;thread<thread_count;thread++)
			pthread_create(&thread_handles[thread],NULL, function, (void*)thread);
	for(thread=0; thread<thread_count; thread++)
		pthread_join(thread_handles[thread],NULL);
	gettimeofday(&end, NULL);
	printf("lock_type: %d, thread_count: %d, time: %ld us \n",LOCK_TYPE, thread_count, get_time_interval(start,end));	
//	printf("%d \n", counter_get_value(&counter));
	}
	return 0;
}

void *function (void *rank)
{
	/*test1 insert 1000000 items
	int i;
	for(i = 0 ;i<MAX;i++)
	list_insert(&aList,i);*/
	//test2 insert 100000 and then delete them
        int i;
/*	for(i = 0 ;i<MAX;i++)
	   list_insert(&aList,i);*/
//        printf("%X\n",aList.head);
//	printf(" rank : %ld\n",(long)rank);
	for(i = 0;i<MAX;i++)
	  list_delete(&aList,i);
	/*test 3 insert and delete randomly
	int sign,i;
	srand(time(0));
	for(i = 0 ;i<MAX;i++)
	{
	  int tmp = rand()%20170;
	  int sign = rand()%2;
	  if(sign)list_insert(&aList,tmp);
	  else list_insert(&aList,tmp);
	}*/
	return NULL;
}
