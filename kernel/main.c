/** @file main.c
 *
 * @brief kernel main
 *
 * @author 
 *	   
 *	   
 * @date   
 */
 
#include <kernel.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <config.h>

#include <exports.h>

// #include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/swi.h>
//#include "kernel_helpers.h"

extern int switch_user_app(int, char **);
//extern void swi_handler(void *);
//extern void irq_handler(void *);

uint32_t global_data;

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{

    //void *user_app_addr = (void *)USR_START_ADDR;

	app_startup(); /* bss is valid after this point */
    global_data = table;

    init_interrupt();
    init_swi();
    init_timer();

    if (VERBOSE) {
    	printf("Kernel Successfully Initialized!\n");
    }
	return switch_user_app(argc, argv);	

		
	assert(0);        /* should never get here */
}
