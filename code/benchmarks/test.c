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
int main(int argc, char **argv) {

	/* Implement HERE */
	mypthread_t test = 0;
  pthread_attr_t* attr = NULL;
  mypthread_create(&test, attr, NULL, NULL);
	mypthread_create(&test, attr, NULL, NULL);

	print();
	return 0;
}
