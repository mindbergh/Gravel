/** @file ctx_switch.c
 *
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */


#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include "sched_i.h"

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static __attribute__((unused)) tcb_t* cur_tcb; /* use this if needed */

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle __attribute__((unused)))
{
    idle->native_prio = IDLE_PRIO;
    idle->cur_prio = IDLE_PRIO;
    idle->context.r4 = (uint32_t)idle;
    idle->context.r5 = (uint32_t)NULL;
    idle->context.r6 = (uint32_t)NULL;
    idle->context.lr = launch_task;

    idle->holds_lock = 0;
    idle->sleep_queue = NULL;
    // TODO smae problem for kstack here
    //
    runqueue_add(idle, idle->native_prio);
}


/**
 * @brief Context switch to the highest priority task while saving off the
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
	// by Ming
	// Unsure: Need to disinterputs?
	tcb_t *old_tcb;
	runqueue_add(cur_tcb, cur_tcb->native_prio);
	tcb_t *task_to_switch = runqueue_remove(highest_prio());
    if (task_to_switch != cur_tcb) {
        old_tcb = cur_tcb;
        cur_tcb = task_to_switch;
        ctx_switch_full(&(task_to_switch->context), &(old_tcb->context));
    }
}

/**
 * @brief Context switch to the highest priority task that is not this task --
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
	tcb_t *task_to_switch = runqueue_remove(highest_prio());
	cur_tcb = task_to_switch;
	ctx_switch_half(&(task_to_switch->context));
}


/**
 * @brief Context switch to the highest priority task that is not this task --
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
	// by Ming
	// Unsure: Need to disinterputs?
	tcb_t *task_to_switch = runqueue_remove(highest_prio());
	tcb_t *tmp = cur_tcb;
	cur_tcb = task_to_switch;
	//runqueue_add(tmp, tmp->native_prio);
	ctx_switch_full(&(task_to_switch->context), &(tmp->context));
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
    return cur_tcb->cur_prio;
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
    return cur_tcb;
}
