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
void function1() {
	for (int i=0; i<3; i++) {
		printf("... function1 %d \n", i);
		sleep(1);
	}
	printf("... function1 exiting \n");
	mypthread_exit(NULL);
}

void function2() {
	for (int i=0; i<3; i++) {
		printf("... function2 %d \n", i);
		sleep(1);
	}
	printf("... function2 exiting \n");
	mypthread_exit(NULL);
}

void function3() {
	for (int i=0; i<3; i++) {
		printf("... function3 %d \n", i);
		sleep(1);
	}
	printf("... function3 exiting \n");
	mypthread_exit(NULL);
}

//testing the atomics without calling from within a thread
void test_mutex(){
	printf("mutex part start\n"); 
	mypthread_mutex_t mutex;
	printf("calling init\n"); 
	mypthread_mutex_init(&mutex, NULL); 
	if(mutex.locked){
		printf("this mutex is locked now\n"); 
	}
	mypthread_mutex_lock(&mutex); 
	mypthread_mutex_lock(&mutex); 
	printf("mutex part done\n"); 
}


int main(int argc, char **argv) {
	test_mutex();

	mypthread_t thread1;
	mypthread_t thread2;
	mypthread_t thread3;
	mypthread_create(&thread1, NULL, function1, NULL);
	mypthread_create(&thread2, NULL, function2, NULL);
	mypthread_create(&thread3, NULL, function3, NULL);
	
	printf("calling join \n");
	mypthread_join(thread3, NULL);
	mypthread_join(thread2, NULL);
	mypthread_join(thread1, NULL);
	
	printf("returning from test bench :) \n");
	return 0;
}
