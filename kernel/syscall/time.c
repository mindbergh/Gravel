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
uint32_t sys_time;   // system time incremented every 1min



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
    uint32_t sec = sys_time;
    uint32_t this_clock = reg_read(OSTMR_OSCR_ADDR);
    uint32_t msec;
    uint32_t res;


    msec = sub(this_clock, last_clock) / OSTMR_FREQ_KHZ; // overflow-free sub
    //printf("systime: %u, msec: %u\n", sys_time, msec);

    if (sys_time > sec) {
        this_clock = reg_read(OSTMR_OSCR_ADDR);
        res = sys_time * MILLIS_IN_MINUTE + (sub(this_clock, last_clock) / OSTMR_FREQ_KHZ);
        return res; 
    } else {
        res = sys_time * MILLIS_IN_MINUTE + msec;
        return res;
    }
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep_syscall(unsigned long millis  __attribute__((unused)))
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
    update_timer(TIMER_0, MILLIS_IN_MINUTE);  // update every second

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
void update_timer(int channel, uint32_t millis) {
    if (VERBOSE)
        printf("Entering update timer\n");
    if (channel == TIMER_0 || channel == TIMER_1) {
        uint32_t time_in_hz = millis * OSTMR_FREQ_KHZ;
        uint32_t final_time = reg_read(OSTMR_OSCR_ADDR) + time_in_hz;
        reg_write(OSTMR_OSMR_ADDR(channel), final_time); // update match reg
    }
    if (VERBOSE)
        printf("Exiting update timer\n");
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
