/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>
#include "mypthread.h"

typedef struct Nodes {
    struct Node* next;
    tcb* value;
} Node;

// A structure to represent a queue
struct Queue {
    int front, rear, size;
    Node* head;
    Node* tail;
};


 
struct Queue* createQueue();

int isEmpty(struct Queue* queue);

int queueSize(struct Queue* queue); 

void enqueue(struct Queue* queue, tcb* item);

tcb* dequeue(struct Queue* queue);

tcb* front(struct Queue* queue);

tcb* rear(struct Queue* queue);

tcb* tcb_by_id(struct Queue* queue, uint target_id);

void remove_by_id(struct Queue* queue, unsigned int target_id); 

void print(struct Queue* queue); 