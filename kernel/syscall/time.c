/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */
#include <exports.h>
#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <syscall.h>
#include <arm/reg.h>
#include <arm/interrupt.h>

uint32_t last_clock;
volatile uint32_t sys_time;   // system time incremented every 1min
//uint32_t time_in_hz;


static inline uint32_t sub(uint32_t this, uint32_t that);


/*
 * Get the time from the os
 * This function get minute from sys_time global variable
 * and add more precise time by compute clock circle
 * This function also take into account the overflow of
 * OSTMR_OSCR. 
 */
unsigned long time_syscall(void)
{
    return sys_time;
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep_syscall(unsigned long millis)
{
	uint32_t deadline = time_syscall() + millis;
    uint32_t now = time_syscall();
    while (deadline > now) {
        now = time_syscall();
    }
}


/* Initial system time related variable and timers
 * It will set a 1 min timer for incrementing system time
 */
void init_timer() {
    sys_time = 0;
    last_clock = reg_read(OSTMR_OSCR_ADDR);
    if (VERBOSE)
        printf("Entering init timer\n");
    update_timer(TIME_RESOLUTION);  

    /* enable channel 0 */
    reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
    reg_clear(INT_ICLR_ADDR, 1 << INT_OSTMR_0);     // make it irq
    reg_set(INT_ICMR_ADDR, 1 << INT_OSTMR_0);       // unmask it
    if (VERBOSE)
        printf("Exiting init timer\n");    

}

/*
 * Update timer registers for time system call 
 */
inline void update_timer(uint32_t millis) {
    uint32_t final_time = reg_read(OSTMR_OSCR_ADDR) + millis * OSTMR_FREQ_KHZ;
    reg_write(OSTMR_OSMR_ADDR(TIMER_0), final_time); // update match reg
}


/*
 * A underflow free subtraction function for uint32
 */
static inline uint32_t sub(uint32_t this, uint32_t that) {
    if (this < that) {
        // overflow occurred
        return ((UINT32_MAX - that) + 1 + this);
    } else {
        return (this - that);
    }
}
