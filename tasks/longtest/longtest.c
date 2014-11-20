/** @file longtest.c
 *
 * @brief A test to compare value of time
 *        accquired by counter register and time() system call
 * @author Ming Fang <mingf@cs.cmu.edu>
 * @author Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @date   2014-11-07
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PERIPHERAL_BASE       0x40000000
#define OSTMR_OSCR_ADDR       0x00A00010   /* OS Timer Count Register */
#define OSTMR_FREQ_KHZ        3686.4       /* Oscillator frequency in Khz */

uint32_t reg_read(size_t addr) {
    return *((volatile uint32_t*)(PERIPHERAL_BASE + addr));
}

/*
 * A underflow free subtraction function for uint32
 */
uint32_t sub(uint32_t this, uint32_t that) {
    if (this < that) {
        // overflow occurred
        return ((UINT32_MAX - that) + 1 + this);
    } else {
        return (this - that);
    }
}


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: go %s <sleep time in ms>\n", argv[0]);
    }
    uint32_t end_clock = 0;
    uint32_t end_clock_time;
    uint32_t end_time;
    uint32_t start_clock = reg_read(OSTMR_OSCR_ADDR);
    uint32_t start_time = time();
    printf("OS clock start: %u\n", start_clock);


    sleep(atoi(argv[1]));

    end_clock = reg_read(OSTMR_OSCR_ADDR);
    end_time = time();

    end_clock_time = sub(end_clock,start_clock) / OSTMR_FREQ_KHZ;
    printf("OS clock end: %u\n", end_clock);
    printf("OS clock time interval: %u\n", end_clock_time);
    printf("Sys call time interval: %u\n", end_time - start_time);
    return -1;
}

