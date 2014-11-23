/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Harry Q Bovik < PUT YOUR NAMES HERE
 *
 *
 * @date
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

mutex_t gtMutex[OS_NUM_MUTEX];
static int cur_mutex_num;

void mutex_init()
{
    int i;
    for (i = 0; i < OS_NUM_MUTEX; i++) {
        gtMutex[i].bAvailable = TRUE;
        gtMutex[i].pHolding_Tcb = NULL;
        gtMutex[i].bLock = FALSE;
        gtMutex[i].pSleep_queue = NULL;
    }
    cur_mutex_num = 0;
}

int mutex_create(void)
{
    disable_interrupts();
    if (cur_mutex_num == OS_NUM_MUTEX) {
        enable_interrupts();
        return -ENOMEM;
    }
    /* bAvailable = TRUE means it has been created */
    gtMutex[cur_mutex_num].bAvailable = FALSE;
    cur_mutex_num++;
    enable_interrupts();
    return cur_mutex_num;
}

int mutex_lock(int mutex  __attribute__((unused)))
{
    tcb_t *cur_tcb, *tcb_iterator;
    mutex_t *cur_mutex;

    disable_interrupts();

    /* check whether the mutex number is valid */
    if (mutex < 0 || (mutex >= OS_NUM_MUTEX - 1) ||
        ((cur_mutex = &(gtMutex[mutex])) && cur_mutex->bAvailable)) {
        enable_interrupts();
        return -EINVAL;
    }

    cur_tcb = get_cur_tcb();
    /* the current task should not have held this mutex */
    if (cur_tcb == cur_mutex->pHolding_Tcb) {
        enable_interrupts();
        return -EDEADLOCK;
    }

    if (cur_mutex->bLock) {
        /* put this tcb into the sleep queue */
        cur_tcb->sleep_queue = NULL;
        tcb_iterator = cur_mutex->pSleep_queue;
        if (tcb_iterator == NULL) {
            cur_mutex->pSleep_queue = cur_tcb;
        } else {
            while (tcb_iterator->sleep_queue) {
                tcb_iterator = tcb_iterator->sleep_queue;
            }
            tcb_iterator->sleep_queue = cur_tcb;
        }
        dispatch_sleep();
    }
    /*
     * TODO
     * I am not sure if the task will return from dispatch_sleep()?
     * If so, should we call get_cur_tcb() again?
     */
    cur_mutex->bLock = TRUE;
    cur_mutex->pHolding_Tcb = cur_tcb;
    /* remove from the sleep list */
    if (cur_mutex->pSleep_queue == cur_tcb) {
        // TODO
    }


    enable_interrupts();
	return 1; // fix this to return the correct value
}

int mutex_unlock(int mutex)
{
    tcb_t *cur_tcb, *tcb_iterator;
    mutex_t *cur_mutex;

    disable_interrupts();

    /* check whether the mutex number is valid */
    if (mutex < 0 || (mutex >= OS_NUM_MUTEX - 1) ||
        ((cur_mutex = &(gtMutex[mutex])) && cur_mutex->bAvailable)) {
        enable_interrupts();
        return -EINVAL;
    }

    cur_tcb = get_cur_tcb();
    /* the current task should be holding this mutex */
    if (cur_tcb != cur_mutex->pHolding_Tcb) {
        enable_interrupts();
        return -EPERM; /* Operation not permitted */
    }

    

    /* To waken or not to waken */
    if (cur_mutex.pSleep_queue != NULL) {
        /* Put the 1st task in the sleep queue into run queue */    
        tcb_t *task_to_waken = cur_mutex->pSleep_queue;
        cur_mutex->pSleep_queue = task_to_waken->sleep_queue;
        cur_mutex->pHolding_Tcb = task_to_waken;
        cur_mutex->bLock = TRUE;
        runqueue_add(task_to_waken, task_to_waken->cur_prio); 
    } else {
        /* No task is waiting */
        cur_mutex->bLock = FALSE;
    }
    enable_interrupts();
	return 0; // fix this to return the correct value
}

