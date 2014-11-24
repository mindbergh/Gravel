/*
 * kernel_helpers.c: other kernel functions
 *
 * Author: Hsueh-Hung Cheng <xuehung@gmail.com>
 * Author: Ming Fang <mingf@cs.cmu.edu>
 * Date:   10/20/2014 14:38
 */

#include <exports.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/reg.h>
#include <bits/swi.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <syscall.h>
#include <types.h>
#include <config.h>
#include <device.h>
#include <lock.h>



/* save the addresses of handlers */
unsigned *uboot_handler_addr[NUM_EXCEPTIONS - 1];
/* only backup the first two instructions */
unsigned instruction_backup[NUM_EXCEPTIONS - 1][2];


/*
 * This is the handler for IRQ interrupt
 * It first find which devise triggered the interrupt and
 * do corresponding task
 */
void C_IRQ_handler(void) {
    if (VERBOSE)
        printf("C_IRQ_handler: entering\n");

    /* find out if the timer cause this interrupt */
    if (reg_read(INT_ICPR_ADDR) & (1 << INT_OSTMR_0)) {
        //puts("C_IRQ_handler: OSTMR_0\n");
        //printf("ICMR_0 caused this interupt\n");
        sys_time++;
        last_clock = reg_read(OSTMR_OSCR_ADDR);
        /* write 1 to this bit to acknowledge the match and clear it */
        reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
        update_timer(TIMER_0, MILLIS_IN_MINUTE);
    } else if (reg_read(INT_ICPR_ADDR) & (1 << INT_OSTMR_1)) {
        if (VERBOSE)
            puts("C_IRQ_handler: OSTMR_1\n");
        /* write 1 to this bit to acknowledge the match and clear it */
        reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M1);
        dev_update(time_syscall());
    }

//    printf("C_IRQ_handler: exiting\n");
}

/*
 * This dispatch the appropriate syscalls.
 * Return the return value of the syscall.
 */
void swi_dispatch(unsigned int swi_number, struct ex_context* c) {
    dbg_printf("entering C_SWI_handler with swi # = %x\n", swi_number);
    if (VERBOSE)
        printf("entering C_SWI_handler with swi # = %x\n", swi_number);
    switch (swi_number) {
        case READ_SWI:
            c->r0 = read_syscall(c->r0, (char *)c->r1, c->r2);
            break;
        case WRITE_SWI:
            c->r0 = write_syscall(c->r0, (char *)c->r1, c->r2);
            break;
        case TIME_SWI:
            c->r0 = time_syscall();
            break;
        case SLEEP_SWI:
            sleep_syscall((unsigned)c->r0);
            break;
        case MUTEX_CREATE:
            c->r0 = mutex_create();
        case MUTEX_LOCK:
            mutex_lock(c->r0);
        case MUTEX_UNLOCK:
            mutex_unlock(c->r0);
        case CREATE_SWI:
            c->r0 = task_create((task_t *)c->r0, (size_t)c->r1);
            break;
        case EVENT_WAIT:
            event_wait((unsigned int)c->r0);
            break;
        default:
            invalid_syscall(swi_number);
    }
}


/*
 * wrie in our own handler given the vecotr
 */
int wiring_handler(unsigned vec_num, void *handler_addr) {
//    printf("entering wiring_handler with vec_num = %x\n", vec_num);
    unsigned *vector_addr = (unsigned *)(vec_num * 0x4);
    /*
     * check SWI vector contains the valid format:
     * that is ldr pc, [pc, positive #imm12]
     */
    if (VERBOSE) {
        printf("%x\n", *vector_addr);
    }
    unsigned offset = (*vector_addr) ^ 0xe59ff000;
    if (offset > 0xfff) {
        return -1;
    }
    /* pc is increased by 0x8 when being accessed */
    int *ptr_to_handler_addr = (void *)(offset + (unsigned)vector_addr + 0x08);
    uboot_handler_addr[vec_num] = (void *)(*ptr_to_handler_addr);

    /* backup the original instructions at SWI_handler */
    instruction_backup[vec_num][0] = *(uboot_handler_addr[vec_num]);
    instruction_backup[vec_num][1] = *(uboot_handler_addr[vec_num] + 1);
    dbg_printf("%x addr = %x", vec_num, uboot_handler_addr[vec_num]);
    /* put new instructions at SWI_handler */
    *(uboot_handler_addr[vec_num]) = 0xe51ff004;       // pc, [pc, #-4]
    *(uboot_handler_addr[vec_num] + 1) = (unsigned int)handler_addr;

//    printf("exiting wiring handler\n");
    return 1;
}

/*
 * restore to the original handlers given vector number
 */
void restore_handler(unsigned vec_num) {

    if (vec_num < NUM_EXCEPTIONS) {
        *(uboot_handler_addr[vec_num]) = instruction_backup[vec_num][0];
        *(uboot_handler_addr[vec_num] + 1) = instruction_backup[vec_num][1];
    }
}


void init_interrupt(void) {
    /* wiring IRQ handler */
    if (wiring_handler(EX_IRQ, irq_handler) < 0) {
        printf("The instruction in the vector table is unrecognized\n");

    }

}

void init_swi(void) {
    /* wiring SWI handler */
    if (wiring_handler(EX_SWI, swi_handler) < 0) {
        printf("The instruction in the vector table is unrecognized\n");
    }
}
