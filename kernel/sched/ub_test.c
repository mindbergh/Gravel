/** @file ub_test.c
 *
 * @brief The UB Test for basic schedulability
 *
 * @ Author: Ming Fang <mingf@andrew.cmu.edu>
 * @ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @ date 2014-11-24
 */

//#define DEBUG 0

#include <sched.h>
#ifdef DEBUG
#include <exports.h>
#endif

float bound_table[OS_MAX_TASKS] = {
    1.0000, 0.8284, 0.7798, 0.7568, 0.7435, 0.7348, 0.7286, 0.7241,
    0.7205, 0.7177, 0.7155, 0.7136, 0.7120, 0.7106, 0.7094, 0.7084,
    0.7075, 0.7067, 0.7059, 0.7053, 0.7047, 0.7042, 0.7037, 0.7033,
    0.7028, 0.7025, 0.7021, 0.7018, 0.7015, 0.7012, 0.7010, 0.7007,
    0.7005, 0.7003, 0.7001, 0.6999, 0.6997, 0.6995, 0.6993, 0.6992,
    0.6990, 0.6989, 0.6988, 0.6986, 0.6985, 0.6984, 0.6983, 0.6982,
    0.6981, 0.6980, 0.6979, 0.6978, 0.6977, 0.6976, 0.6975, 0.6975,
    0.6974, 0.6973, 0.6972, 0.6972, 0.6971, 0.6970, 0.6970, 0.6969
};
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
    dbg_printf("Sum util = %x\n", utilization);
    if (utilization > bound_table[num_tasks]) {
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

