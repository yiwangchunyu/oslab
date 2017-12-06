#include <pthread.h>
#include "mutex.c"
#include "spinlock.c"
#include "lock.h"
#include "structure.h"
#ifdef LOCK_TPYE == SPINLOCK
	typedef struct __counter_t {
		int value;
		spinlock_t lock;
	} counter_t;
	
#elif LOCK_TPYE == MUTEX
	typedef struct __counter_t {
		int value;
		mutex_t lock;
	} counter_t;
	
#elif LOCK_TPYE == PTHREAD_SPINLOCK
	typedef struct __counter_t {
		int value;
		pthread_spinlock_t lock;
	} counter_t;
	
#elif LOCK_TPYE == PTHREAD_MUTEX
	typedef struct __counter_t {
		int value;
		pthread_mutex_t lock;
	} counter_t;
	
#endif

void init(counter_t *c) {
	c->value = 0;
	init(&c->lock, 0);
}

void increment(counter_t *c) {
	lock(&c->lock);
	c->value++;
	unlock(&c->lock);
}

void decrement(counter_t *c) {
	lock(&c->lock);
	c->value--;
	unlock(&c->lock);
}
 
int get(counter_t *c) {
	lock(&c->lock);
	int rc = c->value;
	unlock(&c->lock);
	return rc;
}
