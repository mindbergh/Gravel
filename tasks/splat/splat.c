/** @file splat.c
 *
 * @brief Displays a spinning cursor.
 *
 * Links to libc.
 */
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    while (1) {
        printf("\b \b|");
        sleep(200);
        printf("\b \b/");
        sleep(200);
        printf("\b \b-");
        sleep(200);
        printf("\b \b\\");
        sleep(200);
    }
	return 0;
}
