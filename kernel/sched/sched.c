/** @file sched.c
 *
 * @brief Top level implementation of the scheduler.
 *
 * @ Author: Ming Fang <mingf@andrew.cmu.edu>
 * @ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @ date 2014-11-24
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
    int i, j;
    sched_context_t* ctx;
    /* initialize run queue */
    dbg_printf("allocate_tasks: calling runqueue_init\n");
    runqueue_init();

    /* initialization of tcb */
    dbg_printf("allocate_tasks: initializing tcbs\n");
    for (j = num_tasks - 1; j >= 0; j--) {
        i = j + 1; // shift one to leave room for the highest prio 0.
        ctx = &system_tcb[i].context;
        system_tcb[i].native_prio = i;
        system_tcb[i].cur_prio = i;

        /*
         * r4 = pc, r5 = first argument, r6 = sp
         * for detail, please check launch_task(void)
         */

        ctx->r4 = (uint32_t)tasks[j]->lambda;
        ctx->r5 = (uint32_t)tasks[j]->data;
        ctx->r6 = (uint32_t)tasks[j]->stack_pos;
        ctx->r8 = (uint32_t)global_data;
        ctx->sp = (void *)system_tcb[i].kstack_high;
        ctx->lr = (void *)launch_task;

        system_tcb[i].holds_lock = 0;
        system_tcb[i].sleep_queue = NULL;
        //printf("i = %d, data = %d\n", i, (int)tasks[j]->data);
        /* setup the runqueue */
        runqueue_add(&(system_tcb[i]), i);
    }
    /* Init idle task */

    system_tcb[IDLE_PRIO].native_prio = IDLE_PRIO;
    system_tcb[IDLE_PRIO].cur_prio = IDLE_PRIO;
    system_tcb[IDLE_PRIO].holds_lock = 0;
    ctx = &system_tcb[IDLE_PRIO].context;

    ctx->r4 = 0;
    ctx->r5 = 0;
    ctx->r6 = 0;
    ctx->r7 = 0;
    ctx->r8 = global_data;
    ctx->r9 = 0;
    ctx->r10 = 0;
    ctx->r11 = 0;
    ctx->sp = system_tcb[IDLE_PRIO].kstack_high;
    ctx->lr = &idle;

    runqueue_add(&(system_tcb[IDLE_PRIO]), IDLE_PRIO);
    /* setup for the idle */
    dbg_printf("allocate_tasks: calling dispatch_init\n");
}

