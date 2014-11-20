/** @file write.S
 *
 * @brief write sycall wrapper
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 07 Oct 2007 01:53:46 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-29
 */

#include <asm.h>
#include <bits/swi.h>

	.file "write.S"

FUNC(write)
		@ Add code here.  Put your names at the top.
        swi WRITE_SWI
        cmp r0, #0      @ check if error occurs
        bpl return
        neg r0, r0      @ get negative of return value
        ldr r3, =errno  @ load the address of variable errno
        str r0, [r3]    @ set appropriate errno value
        mov r0, #-1
return:
        mov pc, lr
