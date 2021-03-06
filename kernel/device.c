/**
 * @ file device.c
 *
 * @ brief Implements simulated devices.
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @author Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @date 2014-12-4
 */

#include <types.h>
#include <assert.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/timer.h>
#include <syscall.h>
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
        devices[i].next_match = time_syscall() + dev_freq[i];
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
void dev_wait(unsigned int dev)
{
    dbg_printf("dev_wait(%d): entering\n", dev);
    disable_interrupts();

    dev_t *devc = &(devices[dev]);
    tcb_t *cur_tcb = get_cur_tcb();
    dbg_printf("dev_wait: %u call wait for %u\n", cur_tcb->cur_prio, dev);
    /* insert into the first place */
    cur_tcb->sleep_queue = devc->sleep_queue;
    devc->sleep_queue = cur_tcb;
    dbg_printf("dev_wait: adding prio %d to device %d\n", cur_tcb->cur_prio, dev);

    enable_interrupts();
    /* let current task sleep */
    dispatch_sleep();

}


/**
 * @brief Signals the occurrence of an event on all applicable devices.
 * This function should be called on timer interrupts to determine that
 * the interrupt corresponds to the event frequency of a device. If the
 * interrupt corresponded to the interrupt frequency of a device, this
 * function should ensure that the task is made ready to run
 */
void dev_update(unsigned long millis)
{
    dbg_printf("dev_update: entering\n");
    /*
     * determine which device should be updated
     * if the event of a device has accured, do
     *  1) make all tasks on the sleep_queue ready to run
     *  2) update device
     */
    int i;
    bool_e need_ctx = FALSE;
    dev_t *device;
    tcb_t *tcb_iterator, *tmp_tcb;

    dbg_printf("dev_update: Entering\n");

    disable_interrupts();

    for (i = 0; i < NUM_DEVICES; i++) {
        device = &(devices[i]);
        if (device->next_match <= millis) {
            dbg_printf("dev_update: %d device matches\n", i);
            /* step 1 */
            dbg_printf("dev_update: dev %d match!\n", i);
            tcb_iterator = device->sleep_queue;
            while (tcb_iterator != NULL) {
                need_ctx = TRUE;
                runqueue_add(tcb_iterator, tcb_iterator->cur_prio);
                tmp_tcb = tcb_iterator;
                tcb_iterator = tcb_iterator->sleep_queue;
                tmp_tcb->sleep_queue = NULL;
            }

            device->sleep_queue = NULL;
            dbg_printf("dev_update: finish adding tasks waiting for %d device \n", i);
            /* step 2 */
            device->next_match += dev_freq[i];
        }
    }

    dbg_printf("dev_update: Exiting\n");
    if (need_ctx) {
        dbg_printf("dev_update: need_ctx, calling dispatch_save\n");
        dispatch_save();
    }
    enable_interrupts();
}

