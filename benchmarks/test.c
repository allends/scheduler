#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"
#include "../queue.h"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
void function1() {
	for (int i=0; i<5; i++) {
		printf("... function1 %d \n", i);
		sleep(1);
	}
	printf("... function1 exiting \n");
	mypthread_exit(NULL);
}

void function2() {
	for (int i=0; i<5; i++) {
		printf("... function2 %d \n", i);
		sleep(1);
	}
	printf("... function2 exiting \n");
	mypthread_exit(NULL);
}

void function3() {
	for (int i=0; i<5; i++) {
		printf("... function3 %d \n", i);
		sleep(1);
	}
	printf("... function3 exiting \n");
	mypthread_exit(NULL);
}

void test_mutex(){

}


int main(int argc, char **argv) {

	/* Implement HERE */
	mypthread_t thread1;
	mypthread_t thread2;
	mypthread_t thread3;
	mypthread_create(&thread1, NULL, function1, NULL);
	mypthread_create(&thread2, NULL, function2, NULL);
	mypthread_create(&thread3, NULL, function3, NULL);
	
	printf("calling join \n");
	mypthread_join(thread1, NULL);
	mypthread_join(thread2, NULL);
	mypthread_join(thread3, NULL);
	
	printf("returning from test bench :) \n");
	return 0;
}
