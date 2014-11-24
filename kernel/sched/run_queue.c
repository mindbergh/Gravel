/** @file run_queue.c
 *
 * @brief Run queue maintainence routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <sched.h>
#include "sched_i.h"


#define SET_BIT(x, y)   (x = x | (1 << y))
#define CLEAR_BIT(x, y) (x = x & (~(1 << y)))

static tcb_t* run_list[OS_MAX_TASKS]  __attribute__((unused));

/* A high bit in this bitmap means that the task whose priority is
 * equal to the bit number of the high bit is runnable.
 */
static uint8_t run_bits[OS_MAX_TASKS/NUM_RUN_GROUP];

/* This is a trie structure.  Tasks are grouped in groups of 8.  If any task
 * in a particular group is runnable, the corresponding group flag is set.
 * Since we can only have 64 possible tasks, a single byte can represent the
 * run bits of all 8 groups.
 */
static uint8_t group_run_bits __attribute__((unused));

/* This unmap table finds the bit position of the lowest bit in a given byte
 * Useful for doing reverse lookup.
 */
static uint8_t prio_unmap_table[]  __attribute__((unused)) =
{

0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/**
 * @brief Clears the run-queues and sets them all to empty.
 */
void runqueue_init(void)
{
	int i;
	group_run_bits = 0;
	for (i = 0; i < OS_MAX_TASKS/8; i++) {
        run_bits[i] = 0;
	}
}

/**
 * @brief Adds the thread identified by the given TCB to the runqueue at
 * a given priority.
 *
 * The native priority of the thread need not be the specified priority.  The
 * only requirement is that the run queue for that priority is empty.  This
 * function needs to be externally synchronized.
 */
void runqueue_add(tcb_t* tcb, uint8_t prio)
{
    dbg_printf("runqueue_add: adding prio %d\n", prio);
	uint8_t group_id = prio / NUM_RUN_GROUP;
	uint8_t bit = prio % NUM_RUN_GROUP;
    /* The cooresponding pro bit should not be set already */
	assert((run_bits[group_id] & (1 << bit)) == 0);
	assert(run_list[prio] == NULL);

    SET_BIT(group_run_bits, group_id);
    SET_BIT(run_bits[group_id], bit);

	run_list[prio] = tcb;
}


/**
 * @brief Empty the run queue of the given priority.
 *
 * @return  The tcb at enqueued at the given priority.
 *
 * This function needs to be externally synchronized. //TODO
 */
tcb_t* runqueue_remove(uint8_t prio)
{
    dbg_printf("runqueue_remove: removing prio %d\n", prio);
	uint8_t group_id = prio / NUM_RUN_GROUP;
	uint8_t bit = prio % NUM_RUN_GROUP;
	tcb_t *ret;
	//dbg_printf("About to remove %u, in %u, ", prio, run_bits[group_id]);
    /* The cooresponding pro bit should be set */
	assert((run_bits[group_id] & (1 << bit)));
	assert(run_list[prio]);

	CLEAR_BIT(run_bits[group_id], bit);
    if (run_bits[group_id] == 0) {
    	CLEAR_BIT(group_run_bits, group_id);
    }
    //dbg_printf("After remove %u\n", run_bits[group_id]);
	ret = run_list[prio];
	run_list[prio] = NULL;

	return ret;
}

/**
 * @brief This function examines the run bits and the run queue and returns the
 * priority of the runnable task with the highest priority (lower number).
 */
uint8_t highest_prio(void)
{
	uint8_t group_id = prio_unmap_table[group_run_bits];
	uint8_t bit = prio_unmap_table[run_bits[group_id]];
	return (group_id * NUM_RUN_GROUP) + bit;
}
