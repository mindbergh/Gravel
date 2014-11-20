/** @file typo.c
 *
 * @brief Echos characters back with timing data.
 *
 * Links to libc.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LINE 255	

int main(int argc, char** argv)
{
	/* Add code here -- put your names at the top. */
    unsigned long start = 0;
	unsigned long now = 0;
	unsigned long elapsed;
	unsigned long decimal;
	char buf[MAX_LINE];
	ssize_t readret; 
	while (1) {
		write(STDOUT_FILENO, ">", 1);

		start = time();

		readret = read(STDIN_FILENO, buf, MAX_LINE);
		now = time();

		elapsed = (now - start) / 1000;
		decimal = ((now - start) % 1000) / 100;
		write(STDOUT_FILENO, buf, readret);
		
		sprintf(buf, "\n%lu.%lus\n", elapsed, decimal);
		write(STDOUT_FILENO, buf, strlen(buf));
		
	}
	return 0;
}
