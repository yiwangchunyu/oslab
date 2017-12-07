#include "structure.h"
#if LOCK_TPYE == SPINLOCK
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

void counter_init(counter_t *c,int value) {
	c->value = value;
	lock_init(&c->lock, 0);
}

void counter_increment(counter_t *c) {
	lock(&c->lock);
	c->value++;
	unlock(&c->lock);
}

void counter_decrement(counter_t *c) {
	lock(&c->lock);
	c->value--;
	unlock(&c->lock);
}
 
int counter_get_value(counter_t *c) {
	lock(&c->lock);
	int rc = c->value;
	unlock(&c->lock);
	return rc;
}
