#include "xchg.c"

typedef struct spinlock
{
	int flag;
}spinlock_t;

void spinlock_init(spinlock_t *lock, int init_value)
{
	lock -> flag = 0;//0 for free, 1 for held
}

void spinlock_acquire(spinlock_t *lock)
{
	while(xchg(&lock -> flag, 1) == 1);
	//spin-wait(do nothing)
}

void spinlock_release(spinlock_t *lock)
{
	lock -> flag = 0;
}


