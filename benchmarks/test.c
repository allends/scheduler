#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"
#include "../queue.h"

mypthread_mutex_t mutex;
int ret = 27; 
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

//testing the atomics without calling from within a thread
void test_atomics(){
	mypthread_mutex_t mutex;
	mypthread_mutex_lock(&mutex); 
	mypthread_mutex_lock(&mutex); 
	printf("mutex part done\n"); 
}

//testing the mutexes in threads
void test_mutexes1(){
	printf("starting thread 1\n"); 
	mypthread_mutex_lock(&mutex);
	mypthread_yield(); 
	mypthread_mutex_unlock(&mutex);
	mypthread_exit(NULL);
}

void test_mutexes2(){
	printf("starting thread 2 \n"); 
	printf("calling lock from thread 2\n");
	mypthread_mutex_lock(&mutex); 
	mypthread_mutex_unlock(&mutex); 
	mypthread_exit(&ret);
}


int main(int argc, char **argv) {
	mypthread_t mthread1;
	mypthread_t mthread2;
	mypthread_mutex_init(&mutex, NULL); 
	mypthread_create(&mthread1, NULL, test_mutexes1, NULL);
	mypthread_create(&mthread2, NULL, test_mutexes2, NULL);

	printf("calling join\n");
	int * ret_val; 
	mypthread_join(mthread2, &ret_val);
	mypthread_join(mthread1, NULL);

	// mypthread_t thread1;
	// mypthread_t thread2;
	// mypthread_t thread3;
	// mypthread_create(&thread1, NULL, function1, NULL);
	// mypthread_create(&thread2, NULL, function2, NULL);
	// mypthread_create(&thread3, NULL, function3, NULL);
	
	// printf("calling join \n");
	// mypthread_join(thread3, NULL);
	// mypthread_join(thread2, NULL);
	// mypthread_join(thread1, NULL);
	printf("retval %d\n", *ret_val); 
	printf("returning from test bench :) \n");
	return 0;
}
