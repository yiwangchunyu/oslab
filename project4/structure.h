#include <pthread.h>
#include "mutex.c"
#include "lock.h"
#if LOCK_TYPE == SPINLOCK
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
	
#elif LOCK_TYPE == MUTEX
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

#elif LOCK_TYPE == PTHREAD_SPINLOCK
	void lock_init(pthread_spinlock_t *lock,int init_value)
	{
		pthread_spin_init(lock, 0);
	}
	void lock(pthread_spinlock_t *lock)
	{
		pthread_spin_lock(lock);
	}
	void unlock(pthread_spinlock_t *lock)
	{
		pthread_spin_unlock(lock);
	}

#elif LOCK_TYPE == PTHREAD_MUTEX
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
		pthread_mutex_unlock(lock);
	}
#endif

