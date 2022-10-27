// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

#include "mypthread.h"
#include "queue.h"
#define NUM_THREADS 5

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
static tcb* active;
static tcb* scheduler;
static ucontext_t main;
static uint current_id = 0;
static uint waiting = 0;
static struct Queue* ready_queue;

void switch_to_scheduler() {
  printf("preempted by signal handler\n");
  printf("scheduling %d \n", active->id);
  enqueue(ready_queue, active);
  swapcontext(&active->context, &scheduler->context);
}

/* create a new thread */
int mypthread_create(mypthread_t *thread, pthread_attr_t *attr,
                     void *(*function)(void *), void *arg) {
  // YOUR CODE HERE
  signal(SIGALRM, switch_to_scheduler);

	if (scheduler == NULL) {
    tcb *new_tcb = malloc(sizeof(tcb));
    getcontext(&new_tcb->context);
    new_tcb->context.uc_stack.ss_sp = new_tcb->stack;
	  new_tcb->context.uc_stack.ss_size = sizeof(new_tcb->stack);
    new_tcb->id = 69;
    makecontext(&new_tcb->context, schedule, 0);
    scheduler = new_tcb;
    ready_queue = createQueue(NUM_THREADS);
  }

  // create a Thread Control Block
  tcb *new_tcb = malloc(sizeof(tcb));
  // create and initialize the context of this thread
  getcontext(&new_tcb->context);
  // allocate heap space for this thread's stack
	new_tcb->context.uc_stack.ss_sp = new_tcb->stack;
	new_tcb->context.uc_stack.ss_size = sizeof(new_tcb->stack);
  new_tcb->context.uc_link = &scheduler->context;
  *thread = current_id;
  new_tcb->id = current_id++;
	makecontext(&new_tcb->context, function, 1, arg);
  new_tcb->status = READY;
  
  // TODO: change this to a queue
  // after everything is all set, push this thread into the ready queue
  printf("scheduling %d \n", new_tcb->id);
  enqueue(ready_queue, new_tcb);

  // switch to the schedulers thread
  swapcontext(&main, &scheduler->context);
  printf("returning from mypthread_create\n");
  return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads
 * to use */
int mypthread_yield() {
  // YOUR CODE HERE

  // change current thread's state from Running to Ready
  // main context of this thread to its thread control block
  // switch from this thread's context to the scheduler's context
  printf("yielding and going back to scheduler \n");
  swapcontext(&active->context, &scheduler->context);
  return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
  // YOUR CODE HERE

  // preserve the return value pointer if not NULL
  // deallocate any dynamic memory allocated when starting this thread
  if (active == NULL) {
    setcontext(&scheduler->context);
  }

  if (active->id == waiting) {
    waiting = 0;
  }

  active->status = UNUSED;
  active = NULL;
  printf("going back to the scheduler \n");
  setcontext(&scheduler->context);
  return;
};

/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
  // YOUR CODE HERE

  tcb* target = tcb_by_id(ready_queue,(uint) thread);
  printf("found: %d \n", target->id);

  waiting = target->id;

  setcontext(&scheduler->context);
  // deallocate any dynamic memory created by the joining thread
  return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                         const pthread_mutexattr_t *mutexattr) {
  // YOUR CODE HERE

  // initialize data structures for this mutex

  return 0;
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
  // YOUR CODE HERE

  // use the built-in test-and-set atomic function to test the mutex
  // if the mutex is acquired successfully, return
  // if acquiring mutex fails, put the current thread on the blocked/waiting
  // list and context switch to the scheduler thread

  return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
  // YOUR CODE HERE

  // update the mutex's metadata to indicate it is unlocked
  // put the thread at the front of this mutex's blocked/waiting queue in to the
  // run queue

  return 0;
};

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
  // YOUR CODE HERE

  // deallocate dynamic memory allocated during mypthread_mutex_init

  return 0;
};

/* scheduler */
static void schedule() {
  // YOUR CODE HERE

  // each time a timer signal occurs your library should switch in to this
  // context

  // be sure to check the SCHED definition to determine which scheduling
  // algorithm you should run
  //   i.e. RR, PSJF or MLFQ
  while (1) {
    sched_RR();
    if (!waiting) {
      swapcontext(&scheduler->context, &main);
    }
  }
}

/* Round Robin scheduling algorithm */
static void sched_RR() {
  // YOUR CODE HERE

  // Your own implementation of RR
  // (feel free to modify arguments and return types)

  // set itimer
  struct itimerval timer;
  timer.it_value.tv_usec = 100;
  timer.it_value.tv_sec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;

  // take a proces off of the ready queue and make it run for a little bit
  printf("taking a process off of the ready queue \n");
  active = dequeue(ready_queue);
  if (active == NULL) {
    printf("no more threads \n");
    return;
  }
  printf("active: %d\n", active->id);
  print(ready_queue);
  // swap context to thread
  printf("switching to %d \n", active->id);
  setitimer(ITIMER_REAL, &timer, NULL);
  swapcontext(&scheduler->context, &active->context);
  printf("back to this context\n");
  // return to here
  // block thread
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF() {
  // YOUR CODE HERE

  // Your own implementation of PSJF (STCF)
  // (feel free to modify arguments and return types)

  return;
}

/* Preemptive MLFQ scheduling algorithm */
/* Graduate Students Only */
static void sched_MLFQ() {
  // YOUR CODE HERE

  // Your own implementation of MLFQ
  // (feel free to modify arguments and return types)

  return;
}

// Feel free to add any other functions you need

// YOUR CODE HERE
