/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @date 2014-11-11
 */

#include <asm.h>
#include <bits/swi.h>

	.file "task_create.S"

FUNC(task_create)
	@ Add code here.  Put your name at the top.
	swi CREATE_SWI
	mov pc, lr
