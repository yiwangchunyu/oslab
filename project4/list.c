#include "structure.h"
typedef struct __node_t
{
	int key;
	struct __node_t *next;
}node_t;
#if LOCK_TYPE == SPINLOCK
typedef struct __list_t
{
	node_t* head;
	spinlock_t lock;

}list_t;
#elif LOCK_TYPE == MUTEX
typedef struct __list_t
{
	node_t* head;
	mutex_t lock;

}list_t;
#elif LOCK_TYPE == PTHREAD_SPINLOCK
typedef struct __list_t
{
	node_t* head;
	pthread_spinlock_t lock;

}list_t;
#elif LOCK_TYPE == PTHREAD_MUTEX
typedef struct __list_t
{
	node_t* head;
	pthread_mutex_t lock;

}list_t;
#endif
void list_init(list_t *list)
{
	lock_init((void*)&list->lock,0);
	list->head=NULL;
}
void list_insert(list_t *list, unsigned int key)
{
	node_t* newnode = (node_t*) malloc(sizeof(node_t));
	newnode->key=key;
	lock(&list->lock);
	newnode->next=list->head;
	list->head=newnode;
	unlock(&list->lock);
}
void list_delete(list_t *list, unsigned int key)

{
	lock(&list->lock);
	node_t* curr = (node_t*)malloc(sizeof(node_t));
	if(list->head == NULL)
	{
	unlock(&list->lock);
	return ;}
	curr=list->head;
	if(curr->key==key)
	{
		list->head = curr->next;
		free(curr);
		return ;
	}//如果head就是要删除的元素
	while(curr->next)
	{
		if((curr->next)->key==key)
		{
			curr->next=(curr->next)->next;
			free(curr);
			break;
		}
		curr=curr->next;
	}//如果要删除的元素不是在head
	unlock(&list->lock);
}
void *list_lookup(list_t *list, unsigned int key)
{
    node_t *curr=list->head;
    lock(&list->lock);
    int flag = 0;
    while(curr)
    {
        if(curr->key==key)
        {flag=1;break;}
        curr=curr->next;
    }
    unlock(&list->lock);
    if(flag) return curr;
    else return NULL;

}
int listsize(list_t *list)
{
    node_t *curr = list->head;
    int len = 0;
    lock(&list->lock);
    while(curr)
    {
        len++;
        curr=curr->next;
    }
    unlock(&list->lock);
    return len;
}
void printlist(list_t* list)
{
    lock(&list->lock);
    node_t* curr = list->head;
    while(curr)
    {
        printf("->%d",curr->key);
        curr=curr->next;
    }
    unlock(&list->lock);
    putchar('\n');
}
