/** @file crt0.S
 *
 * @brief Execution startup routine
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @date   11/11/2014
 */

#include <asm.h>

	.file "crt0.S"

FUNC(_start)
	@ needs code.  Put your names at the top.
	ldr	r0, [sp]	@argc
	add	r1, sp, #4	@argv
	bl	main
