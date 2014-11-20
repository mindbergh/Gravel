@ kernel_asm.S: kernel functions implemented in assembly
@
@ Author: Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
@ Author: Ming Fang <mingf@cs.cmu.edu>
@ Date:   10/20/2014 14:50

#include <bits/swi.h>
#include <arm/exception.h>
#include <arm/psr.h>
#include <config.h>

	.file	"kernel_asm.S"
	.text


	.global	switch_user_app
	.type switch_user_app, %function
@ change to user mode and start from specified address
switch_user_app:
	@ r0 = the address standalone application starts
	@ r1 = argc
	@ r2 = argv

	@ store the context
	stmfd	sp!, {r1-r12, lr}
    @ backup sp
    mov     r4, sp

	@ switch to user mode with IRQs unmasked
	mrs	    r3, cpsr
    bic     r3, r3, #PSR_IRQ
	bic	    r3, r3, #PSR_MODE
	orr	    r3, r3, #PSR_MODE_USR
	msr	    cpsr, r3

	@ set the stack 0xa3000000
	ldr	    sp, =USR_STACK

	mov 	r2, #0  	@NULL
	stmfd	sp!, {r2}
	mov		r2, r0
build_argv:	
	subs 	r2, r2, #1	@argc-1 as index
	blt		load_usr
	@iterate thru all ARGVs
	ldr	r3, [r1, r2, lsl #2]
	stmfd	sp!, {r3}
	b		build_argv
load_usr:
	stmfd 	sp!, {r0}	@argc
	ldr 	pc, =USR_START_ADDR 

	.global back_to_kernel
	.type back_to_kernel, %function
@ return to kernel when exit is called
back_to_kernel:
	@ r0 = exit value

	@ restore the original handler
	stmfd	sp!, {r0-r12, lr}
    mov     r0, #EX_SWI
	bl      restore_handler
    mov     r0, #EX_IRQ
	bl	    restore_handler
	ldmfd	sp!, {r0-r12, lr}

	@ get the address to main
	ldr	    r1, =0xa3000000
	@ restore sp and lr
	ldr	    sp, [r1, #-4]
	ldmfd	sp!, {r1-r12, lr}

	@ return to main with r0 = exit value
	bx	    lr
