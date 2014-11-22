/** @file proc.c
 *
 * @brief Implementation of `process' syscalls
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-12
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>

int task_create(task_t* tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
    //Warning: Don’t assume that user code will be sane
    //
    //Inside the task_create syscall, your code will look at the
    //task_t data structure to learn everything needed about the tasks Your
    //kernel should deal with unexpected input not conforming to
    //specifications??? TODO anyother spec to check?

    int i;
    task_t **tasks_ptr_arr;
    /*
     * Num tasks is greater than the maximum number of tasks
     * the OS supports. One if for special task and one is
     * for idle task.
    */
    if (num_tasks > OS_AVAIL_TASKS) {
        return -EINVAL;
    }
    /* Tasks points to region whose bounds lie outside valid address space. */
    // TODO
    if (1) {
        return -EFAULT;
    }
    /* declare an array of pointers such that it is sortable */
    tasks_ptr_arr = (task_t **)malloc(sizeof(task_t *) * num_tasks);
    for (i = num_tasks - 1; i >= 0 ; i--) {
        tasks_ptr_arr[0] = &(tasks[i]);
    }

    /* The given task set is not schedulable. */
    if (!assign_schedule(tasks_ptr_arr, num_tasks)) {
        free(tasks_ptr_arr);
        return -ESCHED;
    }

    /* setup tcb and idle tcb */
    allocate_tasks(tasks_ptr_arr, num_tasks);

    /* initialize devices */
    dev_init();

    /*
     * After all TCBs have been set up, you must context switch to the
     * highest priority task that has been setup
     */


    /* should never return */
    return 1;
}

int event_wait(unsigned int dev  __attribute__((unused)))
{
  return 1; /* remove this line after adding your code */
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
