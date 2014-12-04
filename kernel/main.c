/** @file main.c
 *
 * @brief kernel main
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @author Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @date 2014-12-4
 */

#include <kernel.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <config.h>
#include <lock.h>

#include <exports.h>

#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/swi.h>

extern int switch_user_app(int, char **);

uint32_t global_data;

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{
	app_startup(); /* bss is valid after this point */
    global_data = table;

    init_interrupt();
    init_swi();
    init_timer();
    mutex_init();

    if (VERBOSE) {
    	printf("Kernel Successfully Initialized!\n");
    }
	return switch_user_app(argc, argv);


	assert(0);        /* should never get here */
}
