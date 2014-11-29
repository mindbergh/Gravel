/** @file config.h
 *
 * @brief A set of common OS configuration flags.  This is to control overall
 * OS behavior and not behavior of a particular modules.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-28
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define OS_TICKS_PER_SEC        100    /* Set the number of ticks in one second */
#define OS_TIMER_RESOLUTION     (1000/OS_TICKS_PER_SEC)  /* Timer resolution in ms */

#define LOAD_ADDR  0xa0000000
#define USR_STACK  0xa3000000

#define USR_START_ADDR        0xa0000000
#define USR_END_ADDR          0xa3000000

#define UBOOT_HANDLER_ADDR 0x5c0009c0
#define KERNEL_ERROR 	   0x0badc0de   /* unrecognized instruction error ret */

#define OS_KSTACK_SIZE        4096
#define OS_USTACK_ALIGN       1024

/* OS_MAX_TASKS must be atleast 8 and must be atmost 64 */
#define IDLE_PRIO             63
#define HIGHEST_PRIO          0
#define OS_MAX_TASKS          64
#define OS_AVAIL_TASKS        62  // one for idle and one for highest
#define NUM_RUN_GROUP          8

/* OS_NUM_MUTEX must be at lease 32 */
#define OS_NUM_MUTEX	32


//#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

#endif /* _CONFIG_H_ */
