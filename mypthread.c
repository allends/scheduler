// File:	mypthread.c

// List all group members' names: 
// iLab machine tested on:

#include "mypthread.h"
#include "queue.h"

#define QUANTUM 1 //if the time quantum is rlly big, everything works fine
#define DEBUG 0

// INITIALIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
static tcb* active;
static tcb* scheduler;
ucontext_t waiting_context;
int waiting = 0; // has anyone called join on us

static uint current_id = 0; // maintain increasing id #'s starting from 0 for scheduler
static struct Queue* ready_queue;
static int thread_count = 0; // number of threads created
static struct itimerval timer; 
static struct itimerval curr_timer; 
static struct itimerval no_timer; 
static mypthread_mutex_t sched_mutex; 

// //TEMP for testing purposes!!!
// void sighand(){
//   printf("\nsignal!!!!! \n"); 
//   switch_to_scheduler(); 
// }

void switch_to_scheduler() {
  setitimer(ITIMER_VIRTUAL, &no_timer, &curr_timer); 
  if (active == NULL) {
    if (DEBUG) {
      printf("active is null \n");
    }
    if(waiting){
      //someone has called join
      swapcontext(&waiting_context, &scheduler->context);
    }
    return; 
  }

  //set status of current thread to READY if it was running
  //otherwise, (if BLOCKED), don't override status
  if(active != NULL){
    if(DEBUG){
      printf("active is not null\n");
      printf("active thread id: %d\n", active->id); 
    }
    if(active->status == RUNNING){
      if(DEBUG){
        printf("convert from running to ready: %d\n", active->id); 
      }
      active->status = READY; 
    }
    enqueue(ready_queue, active); //add this to the end of the queue
    setitimer(ITIMER_VIRTUAL, &curr_timer, NULL);
    if(curr_timer.it_value.tv_sec == 0 && curr_timer.it_value.tv_usec == 0){
      setitimer(ITIMER_VIRTUAL, &timer, NULL); 
    }
    swapcontext(&active->context, &scheduler->context);
  }
  else{
    setitimer(ITIMER_VIRTUAL, &curr_timer, NULL);
    if(curr_timer.it_value.tv_sec == 0 && curr_timer.it_value.tv_usec == 0){
      setitimer(ITIMER_VIRTUAL, &timer, NULL); 
    }
    setcontext(&scheduler->context); 
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
    new_tcb->exit_status = NULL; 
    new_tcb->quantums_run = 0; 
    makecontext(&new_tcb->context, schedule, 0);
    scheduler = new_tcb;
    ready_queue = createQueue();
    
    signal(SIGVTALRM, switch_to_scheduler); // i think we only need to set this the once
    
    // set itimer
    timer.it_value.tv_usec = QUANTUM;
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL); 

    //no_timer.it_value = 0;
    no_timer.it_value.tv_usec = 0;
    no_timer.it_value.tv_sec = 0;
    no_timer.it_interval.tv_sec = 0;
    no_timer.it_interval.tv_usec = 0;
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
	makecontext(&new_tcb->context, (void (*)(void)) function, 1, arg);
  new_tcb->status = READY;
  new_tcb->exit_status = NULL; 
  new_tcb->quantums_run = 0; 

  // after everything is all set, push this thread into the ready queue
  enqueue(ready_queue, new_tcb);
  if(DEBUG){
    printf("scheduling %d \n", new_tcb->id);
    print(ready_queue);
  }

  // we have pushed the thread onto the ready queue so return 
  if(DEBUG){
    printf("returning from mypthread_create\n");
  }
  return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads
 * to use */
int mypthread_yield() {
  if(DEBUG){
    printf("yielding and going back to scheduler \n");
  }
  // manually call the scheduler 
  switch_to_scheduler(); 
  if(DEBUG){
    printf("returning from yield\n"); 
  }
  return 0; 
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
  // preserve the return value pointer if not NULL
  // deallocate any dynamic memory allocated when starting this thread
  mypthread_mutex_lock(&sched_mutex); 
  if(DEBUG){
    printf("calling exit\n"); 
  }
  if (active == NULL) {
    if (DEBUG) {
      printf("There is no active thread to exit on! \n");
    }
    setcontext(&scheduler->context);
  }

  active->status = DONE;
  active->exit_status = value_ptr; 

  if(DEBUG){
    printf("going back from exit \n");
    if(active->exit_status!=NULL){
      printf("%d\n", *active->exit_status); 
    }
  }
  mypthread_mutex_unlock(&sched_mutex); 

  // here we can check it everything is done and then free all the memory
  if (all_done(ready_queue)) {
    destroy_queue(ready_queue);
    free(scheduler);
    scheduler = NULL;
  }

  if(waiting){
    setcontext(&waiting_context);
  }
  else{
    switch_to_scheduler(); 
  }
  return; 
};

/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
  if(DEBUG){
    printf("looking for id %d\n", (uint) thread); 
    printf("queue size: %d", queueSize(ready_queue)); 
  }
  tcb* target = tcb_by_id(ready_queue,(uint) thread);
  
  if(target != NULL) {
    if(DEBUG){
      printf("found join thread: %d \n", target->id);
    }
    while(target->status!=DONE){
      waiting = 1; 
      switch_to_scheduler(); 
    }
    //now the target status is done
    if(value_ptr!= NULL){
      *value_ptr = target->exit_status; 
    }
    if(DEBUG){
      printf("done joining on thread %d\n",(uint) thread); 
    }
    return 0; 

  }
  else{ // we enter here when the thread to join isnt found
    if(DEBUG){
      printf("thread to join not found %d\n",(uint) thread); 
      print(ready_queue);
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
  // if(DEBUG){
  //   if(mutex->locked){
  //     printf("mutex already locked\n"); 
  //   }
  // }
  while(__atomic_test_and_set(&mutex->locked, __ATOMIC_RELAXED)){
    enqueue(mutex->waiting, active); 
    active->status = BLOCKED; 
    // if(DEBUG){
    //   printf("already acquired by thread id%d\n", mutex->locking_thread->id); 
    // }
    //called from within a running thread
    switch_to_scheduler(); 
  }
  //obtained the lock
  mutex->locking_thread = active; 
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
    // if(DEBUG){
    //   printf("released the lock!\n"); 
    // }
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
  setitimer(ITIMER_VIRTUAL, &no_timer, &curr_timer); 
  // each time a timer signal occurs your library should switch in to this
  // context

  // be sure to check the SCHED definition to determine which scheduling
  // algorithm you should run
  //   i.e. RR, PSJF or MLFQ
  if (DEBUG){
    printf("in schedule\n"); 
  }
  while(ready_queue!= NULL && !isEmpty(ready_queue)){
    #ifdef RR
      setitimer(ITIMER_VIRTUAL, &curr_timer, NULL); 
      sched_RR(); 
    #endif
    #ifdef PSJF
      setitimer(ITIMER_VIRTUAL, &curr_timer, NULL); 
      sched_PSJF(); 
    #endif
  }
}

/* Round Robin scheduling algorithm */
static void sched_RR() {
  setitimer(ITIMER_VIRTUAL, &no_timer, &curr_timer); 
  // Your own implementation of RR
  // (feel free to modify arguments and return types)

  // take a process off of the ready queue and make it run for a little bit
  if(DEBUG){
    printf("taking a process off of the ready queue in RR \n");
  }
  tcb * tmp; 

  while((tmp = dequeue(ready_queue)) != NULL && tmp->status != READY){
    enqueue(ready_queue, tmp); 
  }
  if (tmp == NULL) {
    if(DEBUG){
      printf("no more ready threads \n");
    }
    return;
  }

  if(DEBUG){
    printf("switching to: %d\n", tmp->id);
    printf("queue size: %d\n", queueSize(ready_queue)); 
    print(ready_queue);
  }

  // swap context to thread
  active = tmp; 
  active->status = RUNNING; 
  setitimer(ITIMER_VIRTUAL, &curr_timer, NULL); 
  swapcontext(&scheduler->context, &active->context);
  if(DEBUG){
    printf("this is the last line of the RR scheduler \n");
  }
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF() {
  // Your own implementation of PSJF (STCF)
  // (feel free to modify arguments and return types)

  // take a process off of the ready queue and make it run for a little bit
  setitimer(ITIMER_VIRTUAL, &no_timer, &curr_timer); 
  if(DEBUG){
    printf("taking a process off of the ready queue in PSJF \n");
  }

  //keep dequeueing + enqueueing until we get a process that we can run
  tcb* least_run = NULL; 
  if(DEBUG){
    printf("queueSize %d\n", queueSize(ready_queue)); 
    print(ready_queue); 
  }
  for(int i = 0; i< queueSize(ready_queue); i++ ){
    tcb* tmp = dequeue(ready_queue); 
    if(tmp->status == READY){
      if(DEBUG){
        printf("tmp status is ready\n"); 
      }
      if(least_run ==  NULL || least_run->quantums_run > tmp->quantums_run){
        least_run = tmp;
        if(DEBUG){
          printf("least run id %d\n", least_run->id); 
        }
      }
    }
    enqueue(ready_queue, tmp); 
  }
  if(DEBUG){
    printf("least run id: %d\n", least_run->id); 
  }
  if(least_run->status == READY){
    if(DEBUG){
      printf("least run status is ready\n"); 
    }
  }
  active = least_run; 
  remove_by_id(ready_queue, least_run->id); 
  if(active == NULL){
    if(DEBUG){
      printf("no more ready threads in PSJF\n");
    }
    return; 
  }

  active->status = RUNNING; 
  ++active->quantums_run; 
  setitimer(ITIMER_VIRTUAL, &curr_timer, NULL); 
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
