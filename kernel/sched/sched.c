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
    /* commented by Mike
	task_t *idle_task = (task_t*)malloc(sizeof(task_t));
	memset(idle_task, 0, sizeof(task_t));
	idle_task->lambda = idle;
    */


}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */

static void __attribute__((unused)) idle(void)
{
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
void allocate_tasks(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
    int i;

    /* initialize run queue */
    runqueue_init();

    /* initialization of tcb */
    for (i = num_tasks - 1; i >= 0; i++) {

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

        system_tcb[i].holds_lock = 0;
        system_tcb[i].sleep_queue = NULL;

        // TODO I don't understand the purpose of kstatck
        // system_tcb[i].kstack = ??;

        /* setup the runqueue */
        runqueue_add(&(system_tcb[i]), i);
    }



    /* setup for the idle */
    system_tcb[IDLE_PRIO].native_prio = IDLE_PRIO;
    system_tcb[IDLE_PRIO].cur_prio = IDLE_PRIO;
    system_tcb[IDLE_PRIO].context.r4 = (uint32_t)idle;
    system_tcb[IDLE_PRIO].context.r5 = (uint32_t)NULL;
    system_tcb[IDLE_PRIO].context.r6 = (uint32_t)NULL;

    system_tcb[IDLE_PRIO].holds_lock = 0;
    system_tcb[IDLE_PRIO].sleep_queue = NULL;
    // TODO smae problem for kstack here

    runqueue_add(&(system_tcb[IDLE_PRIO]), IDLE_PRIO);
}

