#include "sys_futex.c"
#include "spinlock.c"

typedef struct mutex
{
	int flag;
}mutex_t;

void mutex_init(mutex_t *lock,int init_value)
{
	lock -> flag = 0;//0 for free, 1 for held
}

void mutex_acquire(mutex_t *lock)
{
	if(!xchg(&lock -> flag, 1))
		return;
	//the lock is just available
	else
	{
		sys_futex(&lock -> flag, FUTEX_WAIT, 1, 0, 0, 0);
		//when the lock is unavailable, make the threads sleep rather than spin
		mutex_acquire(lock);
	}
}

void mutex_release(mutex_t *lock)
{
	lock -> flag = 0;
	sys_futex(&lock -> flag, FUTEX_WAKE, 1, 0, 0, 0);
	//now, just wake up
}
	

