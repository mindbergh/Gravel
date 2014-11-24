/** @file ub_test.c
 *
 * @brief The UB Test for basic schedulability
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

//#define DEBUG 0

#include <sched.h>
#ifdef DEBUG
#include <exports.h>
#endif


/**
 * @brief Perform UB Test and reorder the task list.
 *
 * The task list at the end of this method will be sorted in order is priority
 * -- from highest priority (lowest priority number) to lowest priority
 * (highest priority number).
 *
 * @param tasks  An array of task pointers containing the task set to schedule.
 * @param num_tasks  The number of tasks in the array.
 *
 * @return 0  The test failed.
 * @return 1  Test succeeded.  The tasks are now in order.
 */
int assign_schedule(task_t** tasks, size_t num_tasks)
{
    dbg_printf("assign_schedule: entering, num_tasks = %ld\n", num_tasks);

    int i, j;
    float utilization = 0;
    float this_util = 0.0;
    task_t *tmp_task;
    for (i = num_tasks - 1; i >= 0; i--) {
        tmp_task = tasks[i];
        this_util = ((float)tmp_task->C / tmp_task->T);
        dbg_printf("C = %ld, T = %ld, This util is: %x\n",tmp_task->C, tmp_task->T, this_util);
        utilization += this_util;
    }
    float threshold = 0.693; // TODO n(2^(1/n) - 1)
    dbg_printf("Sum util = %x\n", utilization);
    if (utilization > threshold) {        
        return 0;
    }

    dbg_printf("assign_schedule: sorting\n");
    /* sort the tasks according its rate*/
    for (i = num_tasks - 1; i >= 0; i--) {
        for (j = i - 1; j >= 0; j--) {
            if (tasks[j+1]->T < tasks[j]->T) {
                tmp_task = tasks[j+1];
                tasks[j+1] = tasks[j];
                tasks[j] = tmp_task;
            }
        }
    }
    dbg_printf("assign_schedule: exiting\n");
	return 1;
}

