// File:	mypthread.c

// List all group members' names: 
// iLab machine tested on:

#include "mypthread.h"
#include "queue.h"

#define NUM_THREADS 5
#define SCHED_RR 1 // nonzero for RR, 0 for SJF
#define DEBUG 1

// INITIALIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
static tcb* active;
static tcb* scheduler;
static ucontext_t main;
static uint current_id = 0;
static uint waiting = 0;
static struct Queue* ready_queue;
static int thread_count = 0; // number of threads created
static struct itimerval timer; 


void switch_to_scheduler() {
  if(DEBUG){
    printf("switch to scheduler\n");
  }
  //set status of current thread to READY if it was running
  //otherwise, (if BLOCKED), don't override status
  if(active!= NULL){
    if(DEBUG){
      printf("active thread id: %d\n", active->id); 
    }
    if(active->status == RUNNING){
      if(DEBUG){
        printf("convert from running to ready: %d\n", active->id); 
      }
      active->status = READY; 
    }
    enqueue(ready_queue, active); //add this to the end of the queue
    swapcontext(&active->context, &scheduler->context);
  }
  else{
    setcontext(scheduler); 
  }
}

/* create a new thread */
int mypthread_create(mypthread_t *thread, pthread_attr_t *attr,
                     void *(*function)(void *), void *arg) {
	if (scheduler == NULL) {
    if(DEBUG){
      printf("scheduler = null\n"); 
    }
    
    tcb *new_tcb = malloc(sizeof(tcb));
    getcontext(&new_tcb->context);
    new_tcb->context.uc_stack.ss_sp = new_tcb->stack;
	  new_tcb->context.uc_stack.ss_size = sizeof(new_tcb->stack);
    new_tcb->id = current_id++; // the scheduler is id 0
    makecontext(&new_tcb->context, schedule, 0);
    scheduler = new_tcb;
    ready_queue = createQueue();

    signal(SIGALRM, scheduler); // i think we only need to set this the once
    
    // SET UP A TIMER
    // set itimer
    struct itimerval timer;
    timer.it_value.tv_usec = 100;
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
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
  new_tcb->id = current_id;
  current_id++; 
	makecontext(&new_tcb->context, function, 1, arg);
  new_tcb->status = READY;

  // after everything is all set, push this thread into the ready queue
  enqueue(ready_queue, new_tcb);
  if(DEBUG){
    printf("scheduling %d \n", new_tcb->id);
    print(ready_queue); 
  }

  if(DEBUG){
    printf("returning from mypthread_create\n");
  }
  return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads
 * to use */
void mypthread_yield() {
  // change current thread's state from Running to Ready
  // main context of this thread to its thread control block
  // switch from this thread's context to the scheduler's context
  if(DEBUG){
    printf("yielding and going back to scheduler \n");
  }
  //this is only called from within a running thread
  switch_to_scheduler(); 
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
  // preserve the return value pointer if not NULL
  // deallocate any dynamic memory allocated when starting this thread
  if(DEBUG){
    printf("calling exit\n"); 
  }
  if (active == NULL) {
    setcontext(&scheduler->context);
  }

  active->status = DONE;
  ucontext_t waiting = active->waiting_context;   
  active->exit_status = value_ptr; 
  //should we leave the thread on the queue?? at which point do we free all the ready queue?

  active = NULL;

  // if(DEBUG){
  //   printf("going back to the scheduler from exit \n");
  // }
  // setcontext(&scheduler->context);
  setcontext(&waiting); 
  return; 
};

/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
  if(DEBUG){
    printf("looking for id %d\n", (uint) thread); 
  }
  tcb* target = tcb_by_id(ready_queue,(uint) thread);
  
  if(target != NULL) {
    if(DEBUG){
      printf("found join thread: %d \n", target->id);
    }
    while(target->status!=DONE){
      swapcontext(&target->waiting_context, &scheduler->context); 
    }
    //now the target status is done
    //DO STUFF
    value_ptr = &target->exit_status; 
    if(DEBUG){
      printf("done joining on thread %d\n",(uint) thread); 
    }
    return 0; 

  }
  else{
    if(DEBUG){
      printf("thread to join not found %d\n",(uint) thread); 
    }
    return -1; //thread to join not found
  }
};

/* initialize the mutex lock (don't call init twice) */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                         const pthread_mutexattr_t *mutexattr) {
  // initialize data structures for this mutex    
  if(DEBUG){
    printf("start initializing a mutex\n"); 
  }                   
  mutex->locking_thread = NULL; 
  __atomic_clear(&mutex->locked, __ATOMIC_RELAXED);
  if(DEBUG){
    if(mutex->locked){
      printf("mutex already locked somehow"); 
    }
  }
  mutex->waiting = createQueue();  //change when we change the queue
  return 0; // 0 means it worked
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
  // use the built-in test-and-set atomic function to test the mutex
  // if the mutex is acquired successfully, return
  // if acquiring mutex fails, put the current thread on the blocked/waiting
  // list and context switch to the scheduler thread
  if(DEBUG){
    if(mutex->locked){
      printf("mutex already locked\n"); 
    }
  }
  while(__atomic_test_and_set(&mutex->locked, __ATOMIC_RELAXED)){
    enqueue(mutex->waiting, active); 
    active->status = BLOCKED; 
    if(DEBUG){
      printf("already acquired by thread id%d\n", mutex->locking_thread->id); 
    }
    //called from within a running thread
    switch_to_scheduler(); 
  }
  //obtained the lock
  mutex->locking_thread = active; 
  if(DEBUG){
    if(mutex->locked){
      printf("mutex locked\n"); 
    }
    printf("got the lock! thread id:%d\n",mutex->locking_thread->id); 
  }
  return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
  // update the mutex's metadata to indicate it is unlocked
  // put the thread at the front of this mutex's blocked/waiting queue in to the
  // run queue
  
  if(mutex->locking_thread!= NULL && mutex->locking_thread == active){
    __atomic_clear(&(mutex->locked), __ATOMIC_SEQ_CST);
    mutex->locking_thread = NULL;
    // IF NOT EMPTY
    if(mutex->waiting!=NULL){
      tcb* first = dequeue(mutex->waiting); 
      if(first!=NULL){
        first->status = READY; 
      }
    }
    return 0;
  }
  // this wasn't the locking thread
  return -1; 
};

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
  // deallocate dynamic memory allocated during mypthread_mutex_init

  // can't destroy a mutex in use
  if(mutex->locked || mutex->waiting == NULL){
    return -1; 
  }
  //make sure that we don't actually free any tcbs
  free(mutex->waiting); //this will change when we change the queue
  return 0;
};

/* scheduler */
static void schedule() {
  // each time a timer signal occurs your library should switch in to this
  // context

  // be sure to check the SCHED definition to determine which scheduling
  // algorithm you should run
  //   i.e. RR, PSJF or MLFQ
  while(ready_queue!= NULL && !isEmpty(ready_queue)){
    if(SCHED_RR){
      sched_RR();
    }
    else{
      sched_PSJF(); 
    }
  }
}

/* Round Robin scheduling algorithm */
static void sched_RR() {
  // Your own implementation of RR
  // (feel free to modify arguments and return types)

  // take a process off of the ready queue and make it run for a little bit
  if(DEBUG){
    printf("taking a process off of the ready queue in RR \n");
  }

  while((active = dequeue(ready_queue)) != NULL && active->status != READY){
    enqueue(ready_queue, active); 
  }
  if (active == NULL) {
    if(DEBUG){
      printf("no more ready threads \n");
    }
    return;
  }

  if(DEBUG){
    printf("switching to: %d\n", active->id);
    print(ready_queue);
  }

  // swap context to thread
  swapcontext(&scheduler->context, &active->context);
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF() {
  // Your own implementation of PSJF (STCF)
  // (feel free to modify arguments and return types)

  // take a process off of the ready queue and make it run for a little bit
  if(DEBUG){
    printf("taking a process off of the ready queue in PSJF \n");
  }

  //keep dequeueing + enqueueing until we get a process that we can run
  tcb* least_run; 
  for(int i = 0; i<NUM_THREADS; i++){
    tcb* tmp = dequeue(ready_queue);
    if(tmp == NULL){
      return; 
    }
    if(tmp->status == BLOCKED){
      enqueue(ready_queue, tmp); 
    }
    else if(tmp->status = READY){
      if(least_run ==  NULL || least_run->quantums_run > tmp->quantums_run){
        least_run = tmp; 
      }
    }
  }
  active = least_run; 
  if(active == NULL){
    if(DEBUG){
      printf("no more ready threads in PSJF\n");
    }
    return; 
  }
  // while((active = dequeue(ready_queue)) != NULL && active->status != READY){
  //     active = dequeue(ready_queue);
  //     if(active->status == BLOCKED){
  //       enqueue(ready_queue, active); 
  //     }
  // }
  active->status = RUNNING; 
  ++active->quantums_run; 
  swapcontext(&scheduler->context, &active->context);
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
