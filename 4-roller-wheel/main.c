#include "linux/spinlock.h"
#include "linux/wait.h"

spinlock_t roller_lock = __SPIN_LOCK_UNLOCKED;
static DECLARE_WAIT_QUEUE_HEAD(roller_lock);