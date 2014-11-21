/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @date 2014-11-11
 */

#include <asm.h>
#include <bits/swi.h>
	.file "mutex_lock.S"

FUNC(mutex_lock)
	@ Add code here.  Put your name at the top.
	swi MUTEX_LOCK
	mov pc,lr

