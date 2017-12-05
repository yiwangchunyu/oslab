#include "xchg.c"

typedef struct spinlock
{
	int flag;
}spinlock_t;

void spinlock_init(spinclock_t *lock)
{
	lock -> flag = 0;//0 for free, 1 for held
}

void spinlock_acquire(spinclock_t *lock)
{
	while(xchg(&lock -> flag, 1) == 1);
	//spin-wait(do nothing)
}

void spinclock_release(spinclock_t *lock)
{
	lock -> flag = 0;
}


