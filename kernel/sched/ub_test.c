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
int assign_schedule(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
    int i, j;
    float utilization = 0;
    task_t *tmp_task;
    for (i = num_tasks - 1; i >= 0; i--) {
        tmp_task = tasks[i];
        utilization += (tmp_task->C / tmp_task->T);
    }
    float threshold = 0.693; // TODO n(2^(1/n) - 1)
    if (utilization > threshold) {
        return 0;
    }

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
	return 1;
}

