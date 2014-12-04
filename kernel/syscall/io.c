/** @file io.c
 *
 * @brief Kernel I/O syscall implementations
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 *
 * @author Ming Fang <mingf@andrew.cmu.edu>
 * @author Hsueh-Hung Cheng <hsuehhuc@andrew.cmu.edu>
 * @date 2014-12-4
 */

#include <types.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/physmem.h>
#include <syscall.h>
#include <exports.h>
#include <kernel.h>

#define EOT_CHAR 0x04
#define DEL_CHAR 0x7f


/* Read count bytes (or less) from fd into the buffer buf. */
ssize_t read_syscall(int fd, void *buf, size_t count)
{
    char c;
    char *ptr = (char *)buf;
    size_t offset = 0;

    if (fd != STDIN_FILENO) {
        return -EBADF;
    }
    if ((unsigned)buf < RAM_START_ADDR || ((unsigned)buf + count -1) > RAM_END_ADDR) {
        return -EFAULT;
    }


    while (offset != count) {
        switch (c = getc()) {
            case 4:	// EOT
                return offset;
            case 8:	// backspace
            case 127: // delete
                if (offset > 0) {
                    offset--;
                    puts("\b \b");
                }
                break;
            case '\n':
            case '\r':
                ptr[offset] = '\n';
                putc('\n');
                return offset;
            default:
                putc(c);
                ptr[offset++] = c;
                break;
        }
    }
    return offset;
}

/* Write count bytes to fd from the buffer buf. */
ssize_t write_syscall(int fd, const void *buf, size_t count)
{

    size_t nwritten = 0;
    char *ptr = (char *)buf;

    if (((unsigned)buf < RAM_START_ADDR || ((unsigned)buf + count - 1) > RAM_END_ADDR) &&
        ((unsigned)buf + count - 1) > FLASH_END_ADDR)
        return -EFAULT;

    if (fd != STDOUT_FILENO) {
        return -EBADF;
    }

    while (nwritten != count) {
        putc(ptr[nwritten++]);
    }
    return nwritten;
	
}

