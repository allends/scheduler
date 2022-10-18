// File:	mypthread.c

// List all group members' names:
// Emmett Duffy
// Allen Davis-Swing
// Adriennne Yu 
// iLab machine tested on:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE

// a thread_id of 0 means that we are the scheduler
static int next_id = 0;
static node* ready_queue = NULL;
static node* current;

void timerhandler() {
  swapcontext(currentContext, schedulerContext);
}

/* stuff for queues */
void print() {
  node* prev = NULL;
  node* curr = ready_queue;
  while (curr != NULL) {
    if (curr->entry == NULL) {
      printf("null and returning \n");
      return;
    }
    printf("%d -> ", curr->entry->mypthread_id);

    prev = curr;
    curr = curr->next;
  }
  printf("\n");
}

// insert link at the first location
void insert_head(tcb* entry) {
  // create a link
  struct node* link = (struct node*)malloc(sizeof(struct node));

  link->entry = entry;

  // point it to old first node
  link->next = ready_queue;

  // point first to new first node
  ready_queue = link;
}

// insert link at the last location
void insert_tail(tcb* entry) {
  // create a link
  struct node* link = (struct node*)malloc(sizeof(struct node));
  link->entry = entry;

  // find the last node in the list + point to it here
  node* curr = ready_queue;
  node* prev = NULL;
  int count = 0;
  while (curr != NULL) {
    prev = curr;
    curr = curr->next;
    count++;
  }
  prev->next = link;
}

int enqueue(node* new_node) {
  if (next_id > 3) {
    insert_tail(new_node->entry);
    return 0;
  }
  insert_head(new_node->entry);
  return 0;
}

/* create a new thread */
int mypthread_create(mypthread_t* thread, pthread_attr_t* attr,
                     void* (*function)(void*), void* arg) {
  // YOUR CODE HERE
  if (next_id == 0) {
    // we are the main thread here
    // the routine that we run will be the scheduler
    char stack[1024];
    tcb* new_thread_entry = malloc(sizeof(tcb));
    new_thread_entry->mypthread_id = next_id++;
    new_thread_entry->mypthread_priority = 0;
    new_thread_entry->mypthread_status = 0;
    // here the function should be the scheduler
    getcontext(&(new_thread_entry->context));
    new_thread_entry->context.uc_stack.ss_sp = stack;
    new_thread_entry->context.uc_stack.ss_size = sizeof(stack);
    makecontext(&(new_thread_entry->context), NULL, 0);
    printf("here\n");
    // doesn't return since you have to make another thread
  }
  // create a Thread Control Block
  // assumes that the head of the queue is defined
  char stack[1024];
  tcb* new_thread_entry = malloc(sizeof(tcb));
  new_thread_entry->mypthread_id = next_id++;
  new_thread_entry->mypthread_priority = 0;
  new_thread_entry->mypthread_status = 0;
  getcontext(&(new_thread_entry->context));
  new_thread_entry->context.uc_stack.ss_sp = stack;
  new_thread_entry->context.uc_stack.ss_size = sizeof(stack);
  makecontext(&(new_thread_entry->context), function, 0);

  node* new_node = malloc(sizeof(node));
  new_node->next = NULL;
  new_node->entry = new_thread_entry;
  enqueue(new_node);
  // create and initialize the context of this thread
  // allocate heap space for this thread's stack
  // after everything is all set, push this thread into the ready queue

  return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads
 * to use */
int mypthread_yield() {
  // YOUR CODE HERE

  // change current thread's state from Running to Ready
  // save context of this thread to its thread control block
  // switch from this thread's context to the scheduler's context

  return 0;
};

/* terminate a thread */
void mypthread_exit(void* value_ptr) {
  // YOUR CODE HERE

  // preserve the return value pointer if not NULL
  // deallocate any dynamic memory allocated when starting this thread

  return;
};

/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void** value_ptr) {
  // YOUR CODE HERE

  // wait for a specific thread to terminate
  // deallocate any dynamic memory created by the joining thread

  return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t* mutex,
                         const pthread_mutexattr_t* mutexattr) {
  // YOUR CODE HERE

  // initialize data structures for this mutex

  return 0;
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t* mutex) {
  // YOUR CODE HERE

  // use the built-in test-and-set atomic function to test the mutex
  // if the mutex is acquired successfully, return
  // if acquiring mutex fails, put the current thread on the blocked/waiting
  // list and context switch to the scheduler thread

  return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t* mutex) {
  // YOUR CODE HERE

  // update the mutex's metadata to indicate it is unlocked
  // put the thread at the front of this mutex's blocked/waiting queue in to the
  // run queue

  return 0;
};

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t* mutex) {
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
  // if there are no entries on the TCB linked list return?
  swapcontext(schedulerContext, nextContext);
  return;
}

/* Round Robin scheduling algorithm */
static void sched_RR() {
  // YOUR CODE HERE

  // Your own implementation of RR
  // (feel free to modify arguments and return types)

  return;
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
int main() {
  // test create thread here
  mypthread_t test = 0;
  pthread_attr_t* attr = NULL;
  mypthread_create(&test, attr, NULL, NULL);
  mypthread_create(&test, attr, NULL, NULL);
  mypthread_create(&test, attr, NULL, NULL);
  mypthread_create(&test, attr, NULL, NULL);
  mypthread_create(&test, attr, NULL, NULL);
  mypthread_create(&test, attr, NULL, NULL);

  print();
}
