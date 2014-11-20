@ asm_handler.S: Implement handlers including SWI and IRQ
@ 		handlers which will forward control to C code
@
@ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
@ Author: Ming Fang <mingf@cs.cmu.edu>
@ Date:   10/20/2014 14:50

#include <bits/swi.h>
#include <arm/psr.h>
#include <asm.h>

@#include <exports.h>

FUNC(swi_handler)
	@ leave room on stack for SPSR
	sub	sp, sp, #4

	@ store all non-banked user space registers
	stmfd	sp!, {r0-r12, lr}

	ldr r12, =global_data				@Restoring r8 for lib funcs
	ldr r8, [r12]

	mrs r3, cpsr
	bic r3, r3, #PSR_IRQ
	msr cpsr, r3

	@ backup spsr
	mrs	r2, spsr
	str	r2, [sp, #14*4]

	@ pass the stack pointer as the second arg
	mov	r1, sp

	@ determine the SWI number called
	ldr	r0, [lr,#-4]
	bic	r0, r0, #0xff000000

	@ call swi_dispatch
	bl	swi_dispatch

	@ restore spsr
	ldr	r2, [sp, #14*4]
	msr	spsr, r2

	@ restore non-banked user space registers except r0
	@ r0 is the return value from swi_dispatch
	ldmfd	sp!, {r0-r12, lr}

	@ remove space used to store SPSR
	add	sp, sp, #4

	@ return to user space
	movs	pc, lr


/*
 * Code to take an IRQ.
 */
FUNC(irq_handler)
	/* lr starts off pointing at next instruction + 4 -- fix this. */
	sub      lr, lr, #4

	/* No nesting -- this is a temporary stack. */
	ldr      sp, =0xa2000000
	stmfd    sp!, {r0,r1}

	/* Move special regs into r0, r1. */
	mrs      r0, spsr
	mov      r1, lr

	/* Switch to supervisor mode */
	msr      cpsr_c, #(PSR_MODE_SVC | PSR_IRQ)

	/* Diq the spsr_svc because svc-mode (SWI) entry code saves it with IRQ 
	 * disabled.  I feel so dirty doing this.  
	 * We need to diq spsr because we need atomic reload of pc and cpsr
	 * during the return sequence in case we came in from user mode.
	 */
	msr      spsr, r0

	/* Save lr, pop of r0, r1 from irq stack, and then put them all on the 
	 * svc stack along with other caller-save registers.
	 * The stack will look like:
	 * {r0-r3, r8, ip, lr, pc} of source
	 */
	stmfd    sp!, {r1}
	ldr      r1, =0xa1fffff8
	ldm      r1, {r0, r1}
	stmfd    sp!, {r0-r3, r8, ip, lr}

	/* Recover user registers and save */
	mrs      r0, spsr
	stmfd    sp, {r0, sp, lr}^
	add      sp, sp, #-12

	/* Set up r8 correctly with u-boot dispatch table. */
	ldr      ip, =global_data
	ldr      r8, [ip]
	
	/* Call the IRQ handler in C. */
	bl       C_IRQ_handler
	@mov      r0, sp
	@mov      r1, #128
	@bl       hexdump

	/* Restore registers.  Also restore CPSR. */
	ldmfd    sp, {r0, sp, lr}^
	add      sp, sp, #12
	msr      spsr, r0
	ldmfd    sp!, {r0-r3, r8, ip, lr, pc}^
