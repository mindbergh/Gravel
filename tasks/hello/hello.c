/** @file hello.c
 *
 * @brief Prints out Hello world using the syscall interface.
 *
 * Links to libc.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-10-29
 */
#include <unistd.h>

const char hello[] = "Hello World\r\n";

int main(int argc, char** argv)
{
	int i;
	printf("argc = %d\n", argc);

	for (i = 0; i < argc; i++) {
		printf("ARGV%d:%s\n", i, argv[i]);
	}

	write(STDOUT_FILENO, hello, sizeof(hello) - 1);
	return 0;
}
