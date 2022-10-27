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
	for (int i=0; i<5; i++) {
		printf("doing work %d \n", i);
		sleep(1);
		// mypthread_yield();
	}
	mypthread_exit(NULL);
}

void function2() {
	for (int i=0; i<5; i++) {
		printf("doing work2 %d \n", i);
		sleep(1);
		// mypthread_yield();
	}
	mypthread_exit(NULL);
}

int main(int argc, char **argv) {

	/* Implement HERE */
	mypthread_t thread1;
	mypthread_t thread2;
	mypthread_create(&thread1, NULL, function, NULL);
	printf("thread: %d\n", thread1);
	mypthread_create(&thread2, NULL, function2, NULL);
	
	mypthread_join(thread1, NULL);
	mypthread_join(thread2, NULL);
	
	printf("returning from test bench :) \n");
	return 0;
}
