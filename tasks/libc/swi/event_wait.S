/** @file event_wait.S
 *
 * @brief time sycall wrapper
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @date 2014-11-11
 */

#include <asm.h>
#include <bits/swi.h>
	.file "event_wait.S"

FUNC(event_wait)
	@ Add code here.  Put your name at the top.
	swi EVENT_WAIT
	mov pc, lr
