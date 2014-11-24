/** @file sched.c
 *
 * @brief Top level implementation of the scheduler.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

void sched_init(task_t* main_task  __attribute__((unused)))
{
    dbg_printf("sched_init: entering\n");
    /*
     * I don't understand why we need main_task
     */
    dbg_printf("sched_init: calling dispatch_nosave\n");
    dispatch_nosave();
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */

static void __attribute__((unused)) idle(void)
{
    dbg_printf("idle: entering\n");
    enable_interrupts();
    while(1);
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks, size_t num_tasks)
{
    dbg_printf("allocate_tasks: entering\n");
    int i;

    /* initialize run queue */
    dbg_printf("allocate_tasks: calling runqueue_init\n");
    runqueue_init();

    /* initialization of tcb */
    dbg_printf("allocate_tasks: initializing tcbs\n");
    for (i = num_tasks - 1; i >= 0; i--) {

        system_tcb[i].native_prio = i;
        system_tcb[i].cur_prio = i;

        /*
         * r4 = pc, r5 = first argument, r6 = sp
         * for detail, please check launch_task(void)
         * TODO should we initialize other registers in the context?
         */
        system_tcb[i].context.r4 = (uint32_t)tasks[i]->lambda;
        system_tcb[i].context.r5 = (uint32_t)tasks[i]->data;
        system_tcb[i].context.r6 = (uint32_t)tasks[i]->stack_pos;
        system_tcb[i].context.lr = launch_task;

        system_tcb[i].holds_lock = 0;
        system_tcb[i].sleep_queue = NULL;

        // TODO I don't understand the purpose of kstatck
        // system_tcb[i].kstack = ??;

        /* setup the runqueue */
        runqueue_add(&(system_tcb[i]), i);
    }

    /* setup for the idle */
    dbg_printf("allocate_tasks: calling dispatch_init\n");
    dispatch_init(&(system_tcb[IDLE_PRIO]));
}

