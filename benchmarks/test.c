#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
void function() {
	printf("stuff\n");
}

int main(int argc, char **argv) {

	/* Implement HERE */
	mypthread_t thread;
	mypthread_create(&thread, NULL, NULL, NULL);
	mypthread_create(&thread, NULL, NULL, NULL);

	printf("finished \n");
	return 0;
}
