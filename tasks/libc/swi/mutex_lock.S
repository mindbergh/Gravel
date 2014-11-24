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
	cmp r0, #0	@ check if error occurs
	bpl return
	neg r0, r0	@ get negative of return value
	ldr r3, =errno	@ load the address of variable errno
	str r0, [r3]	@ set appropriate errno value
	mov r0, #-1
return:
	mov pc, lr
