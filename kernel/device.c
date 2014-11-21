/**
 * @file device.c
 *
 * @brief Implements simulated devices.
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-12-01
 */

#include <types.h>
#include <assert.h>

#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

/**
 * @brief Fake device maintainence structure.
 * Since our tasks are periodic, we can represent
 * tasks with logical devices.
 * These logical devices should be signalled periodically
 * so that you can instantiate a new job every time period.
 * Devices are signaled by calling dev_update
 * on every timer interrupt. In dev_update check if it is
 * time to create a tasks new job. If so, make the task runnable.
 * There is a wait queue for every device which contains the tcbs of
 * all tasks waiting on the device event to occur.
 */

struct dev
{
	tcb_t* sleep_queue;
	unsigned long   next_match;
};
typedef struct dev dev_t;

/* devices will be periodically signaled at the following frequencies */
const unsigned long dev_freq[NUM_DEVICES] = {100, 200, 500, 50};
static dev_t devices[NUM_DEVICES];

/**
 * @brief Initialize the sleep queues and match values for all devices.
 */
void dev_init(void)
{
    /* the following line is to get rid of the warning and should not be needed */
    // I am not sure what to do, try to initialize devices, by Mike

    int i;
    /*unsigned long curr_time = time();*/
    for (i = 0; i < NUM_DEVICES; i++) {
        devices[i].sleep_queue = NULL;
        // TODO devices[i].next_match = curr_time +
    }
}


/**
 * @brief Puts a task to sleep on the sleep queue until the next
 * event is signalled for the device.
 * This is called by tasks through event_wait
 *
 * @param dev  Device number.
 */
void dev_wait(unsigned int dev __attribute__((unused)))
{

    dev_t *devc = &(devices[dev]);
    tcb_t *cur_tcb = get_cur_tcb();
    // shoud we insert according to the priority ?
    // TODO now only insert into the first place
    cur_tcb->sleep_queue = devc->sleep_queue;
    devc->sleep_queue = cur_tcb;

    // do context switch ? TODO
}


/**
 * @brief Signals the occurrence of an event on all applicable devices.
 * This function should be called on timer interrupts to determine that
 * the interrupt corresponds to the event frequency of a device. If the
 * interrupt corresponded to the interrupt frequency of a device, this
 * function should ensure that the task is made ready to run
 */
void dev_update(unsigned long millis __attribute__((unused)))
{
    /*
     * if the event of a device has accured, do
     * 1) wake up all tasks on the sleep_queue
     * 2) make them ready to run
     * 3) update device
     */
    //TODO
}

