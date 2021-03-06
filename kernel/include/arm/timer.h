/**
 * @file timer.h
 *
 * @brief Definitions for the OS timer.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 *
 * @date 2008-07-07
 *
 * @note The addresses here are the addresses stated in the Intel PXA255
 *       Processor Developer's Manual minus 0x40000000.  This is so that
 *       this memory region can be relocated if we ever turn on the MMU.
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#define OSTMR_OSMR_ADDR(x)    (0x00A00000 + (x)*4)    /* OS Timer Match Register */

#define OSTMR_OSCR_ADDR       0x00A00010   /* OS Timer Count Register */

#define OSTMR_OSSR_ADDR       0x00A00014   /* OS Timer Status Register */
#define OSTMR_OSSR_M0         0x00000001   /* Matched 0 */
#define OSTMR_OSSR_M1         0x00000002   /* Matched 1 */
#define OSTMR_OSSR_M2         0x00000004   /* Matched 2 */
#define OSTMR_OSSR_M3         0x00000008   /* Matched 3 */

#define OSTMR_OWER_ADDR       0x00A00018   /* OS Timer Watchdog Enable Register */
#define OSTMR_OWER_WME        0x00000001   /* Watchdog mode enable */

#define OSTMR_OIER_ADDR       0x00A0001C   /* OS Timer Interrupt Enable Register */
#define OSTMR_OIER_E0         0x00000001   /* Enable match 0 */
#define OSTMR_OIER_E1         0x00000002   /* Enable match 1 */
#define OSTMR_OIER_E2         0x00000004   /* Enable match 2 */
#define OSTMR_OIER_E3         0x00000008   /* Enable match 3 */

#define OSTMR_FREQ            3686400      /* Oscillator frequency in hz */
#define OSTMR_FREQ_KHZ        3686.4       /* Oscillator frequency in Khz */
#define TIME_RESOLUTION       5

#define TIMER_0     0


extern volatile uint32_t sys_time;   // system time incremented every 1min

#ifndef ASSEMBLER

void init_timer(void);
inline void update_timer(uint32_t millis);



void destroy_timer(void);
void timer_handler(unsigned int int_num);
unsigned long get_ticks(void);
unsigned long get_millis(void);

#endif /* ASSEMBLER */

#endif /* _TIMER_H_ */
