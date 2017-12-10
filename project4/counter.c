#include "structure.h"
#define GROUP 4
#if LOCK_TPYE == SPINLOCK
	typedef struct counter_t {
		int value;
		spinlock_t value_lock;
		int group_counter[GROUP];
		spinlock_t lock[GROUP];
	} counter_t;
	
#elif LOCK_TPYE == MUTEX
	typedef struct counter_t {
		int value;
		spinlock_t value_lock;
		int group_counter[GROUP];
		spinlock_t lock[GROUP];
	} counter_t;
	
#elif LOCK_TPYE == PTHREAD_SPINLOCK
	typedef struct counter_t {
		int value;
		spinlock_t value_lock;
		int group_counter[GROUP];
		spinlock_t lock[GROUP];
	} counter_t;
	
#elif LOCK_TPYE == PTHREAD_MUTEX
	typedef struct counter_t {
		int value;
		spinlock_t value_lock;
		int group_counter[GROUP];
		spinlock_t lock[GROUP];
	} counter_t;
	
#endif

void counter_init(counter_t *c,int value) {
	c->value = value;
	int i;
	for(i=0;i<GROUP;i++)
	{
		c->group_counter[i]=0;
		lock_init(&c->lock[i], 0);
	}
		

	lock_init(&c->value_lock, 0);
}

void counter_increment(counter_t *c,int rank) {
	lock(&c->lock[rank%GROUP]);
	c->group_counter[rank%GROUP]++;
	unlock(&c->lock[rank%GROUP]);
}

void counter_decrement(counter_t *c,int rank) {
	lock(&c->lock[rank%GROUP]);
	c->group_counter[rank%GROUP]--;
	unlock(&c->lock[rank%GROUP]);
}
 
int counter_get_value(counter_t *c) {
	lock(&c->value_lock);
	int i;
	for(i=0;i<GROUP;i++)
	{
		lock(&c->lock[i]);
		c->value+=c->group_counter[i];
		c->group_counter[i]=0;
		unlock(&c->lock[i]);
	}
	int rc = c->value;
	unlock(&c->value_lock);
	return rc;
}
