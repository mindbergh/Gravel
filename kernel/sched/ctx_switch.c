/** @file ctx_switch.c
 *
 * @brief C wrappers around assembly context switch routines.
 *
 * @ Author: Ming Fang <mingf@andrew.cmu.edu>
 * @ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @ date 2014-11-24
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
    dbg_printf("dispatch_init: entering\n");
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
    dbg_printf("dispatch_save: entering\n");
	tcb_t *old_tcb;
    dbg_printf("dispatch_save: adding current tcb to runqueue\n");
	runqueue_add(cur_tcb, cur_tcb->native_prio);
	tcb_t *task_to_switch = runqueue_remove(highest_prio());
    if (task_to_switch != cur_tcb) {
        old_tcb = cur_tcb;
        cur_tcb = task_to_switch;
        dbg_printf("\n--\ndispatch_save: full switching from %d to %d\n",
                old_tcb->cur_prio, task_to_switch->cur_prio);
        ctx_switch_full(&(task_to_switch->context), &(old_tcb->context));
        dbg_printf("\tdispatch_save: returning from ctx_switch_full!!\n");
    }
    dbg_printf("dispatch_save: exiting\n");
}

/**
 * @brief Context switch to the highest priority task that is not this task --
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
    dbg_printf("dispatch_nosave: entering\n");
	tcb_t *task_to_switch = runqueue_remove(highest_prio());
	cur_tcb = task_to_switch;
    dbg_printf("dispatch_nosave: half switching to %d\n",
            task_to_switch->cur_prio);
    dbg_printf("r4 = %x\n", task_to_switch->context.r4);
    dbg_printf("r5 = %x\n", task_to_switch->context.r5);
    dbg_printf("r6 = %x\n", task_to_switch->context.r6);
    dbg_printf("lr = %x\n", task_to_switch->context.lr);
    dbg_printf("launch_task = %x\n", launch_task);
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
	tcb_t *task_to_switch = runqueue_remove(highest_prio());
	tcb_t *tmp = cur_tcb;
	cur_tcb = task_to_switch;
    dbg_printf("\n--\ndispatch_sleep: full switching from %d to %d\n",
            tmp->cur_prio, task_to_switch->cur_prio);
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
