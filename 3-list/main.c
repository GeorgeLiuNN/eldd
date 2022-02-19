#include "linux/list.h"
#include "linux/spinlock.h"
#include "linux/wait.h"
#include "linux/sched.h"
#include "linux/slab.h"

static struct _mydrv_wq {
    struct list_head    mydrv_worklist;
    spinlock_t          lock;
    wait_queue_head_t   todo;
} mydrv_wq;

static struct _mydrv_work {
    struct list_head    mydrv_workitem;
    void (*work_func)(void *);
    void *work_data;
} /*mydrv_work*/;

// the work thread
static int 
mydrv_worker(void *unused) {
    DECLARE_WAITQUEUE(wait, current);
    void (*worker_func)(void *);
    void *worker_data;
    struct _mydrv_work *mydrv_work;

    set_current_state(TASK_INTERRUPTIBLE);

    while (1) {
        add_wait_queue(&mydrv_wq.todo, &wait);

        if (list_empty(&mydrv_wq.mydrv_worklist)) {
            schedule();
        } else {
            set_current_state(TASK_RUNNING);
        }
        remove_wait_queue(&mydrv_wq.todo, wait);

        spin_lock(&mydrv_wq.lock);

        while (!list_empty(&mydrv_wq.mydrv_worklist)) {
            mydrv_work = list_entry(mydrv_wq.mydrv_worklist.next, struct _mydrv_work, mydrv_workitem);
            worker_func = mydrv_work->worker_func;
            worker_data = mydrv_work->worker_data;

            list_del(mydrv_wq.mydrv_worklist.next);
            kfree(mydrv_work);

            spin_unlock(&mydrv_wq.lock);
            worker_func(worker_data);
            spin_lock(&mydrv_wq.lock);
        }

        spin_unlock(&mydrv_wq.lock);
        set_current_state(TASK_INTERRUPTIBLE);
    }
    
    set_current_state(TASK_RUNNING);
    return 0;
}

// initialize data structures
static init __init
mydrv_init(void) {
    spin_lock_init(&mydrv_wq.lock);

    init_waitqueue_head(&mydrv_wq.todo);

    INIT_LIST_HEAD(&mydrv_wq.mydrv_worklist);

    kernel_thread(mydrv_worker, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);

    return 0;
}

// submitting work to be executed later
int 
submit_work(void (*func)(void *data), void *data) {
    struct _mydrv_work *mydrv_work;

    mydrv_work = kmalloc(sizeof(struct _mydrv_work), GFP_ATOMIC);
    if (!mydrv_work) return -1;

    mydrv_work->worker_func = func;
    mydrv_work->data = data;

    spin_lock(mydrv_wq.lock);

    list_add_tail(&mydrv_work->mydrv_workitem, &mydrv_wq.mydrv_worklist);

    wake_up(mydrv_wq.todo);

    spin_unlock(mydrv_wq.lock);

    return 0;
}


// 