#include <pthread.h>
#include "mutex.c"
#include "spinlock.c"
#include "lock.h"
#ifdef LOCK_TPYE == SPINLOCK
	void lock_init(spinlock_t *lock, int init_value)
	{
		spinlock_init(lock, init_value);
	}
	void lock(spinlock_t *lock)
	{
		spinlock_acquire(lock);
	}
	void unlock(spinlock_t *lock)
	{
		spinlock_release(lock);
	}
	
#elif LOCK_TPYE == MUTEX
	void lock_init(mutex_t *lock, int init_value)
	{
		mutex_init(lock, init_value);
	}
	void lock(mutex_t *lock)
	{
		mutex_acquire(lock);
	}
	void unlock(mutex_t *lock)
	{
		mutex_release(lock);
	}

#elif LOCK_TPYE == PTHREAD_SPINLOCK
	void lock_init(pthread_spinlock_t *lock,int init_value)
	{
		pthread_spin_init(lock, NULL);
	}
	void lock(pthread_spinlock_t *lock)
	{
		pthread_spin_lock(lock);
	}
	void unlock(pthread_spinlock_t *lock)
	{
		pthread_spinlock_t(lock);
	}

#elif LOCK_TPYE == PTHREAD_MUTEX
	void lock_init(pthread_mutex_t *lock,int init_value)
	{
		pthread_mutex_init(lock,NULL);
	}
	void lock(pthread_mutex_t *lock)
	{
		pthread_mutex_lock(lock);
	}
	void unlock(pthread_mutex_t *lock)
	{
		pthread_mutex_t(lock);
	}
#endif

void counter_init(counter_t *c, int value);
int counter_get_value(counter_t *c);
void counter_increment(counter_t *c);
void counter_decrement(counter_t *c);

void list_init(list_t *list);
void list_insert(list_t *list, unsigned int key);
void list_delete(list_t *list, unsigned int key);
void *list_lookup(list_t *list, unsigned int key);

void hash_init(hash_t *hash, int size);
void hash_insert(hash_t *hash, unsigned int key);
void hash_delete(hash_t *hash, unsigned int key);
void *hash_lookup(hash_t *hash, unsigned int key);