#define LOCK_TYPE 0
#define SPINLOCK 0
#define MUTEX 1
#define PTHREAD_SPINLOCK 2
#define PTHREAD_MUTEX 3

void spinlock_init(spinclock_t *lock, int init_value);
void spinlock_acquire(spinclock_t *lock);
void spinclock_release(spinclock_t *lock);

void mutex_init(mutex_t *lock,int init_value);
void mutex_acquire(mutex_t *lock);
void mutex_release(mutex_t *lock);