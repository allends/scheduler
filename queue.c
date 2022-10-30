#include "queue.h"

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = malloc(
        queue->capacity * sizeof(tcb*));
    printf("capacity: %d \n", capacity);
    return queue;
}
 
// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, tcb* item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}
 
// Function to remove an item from queue.
// It changes front and size
tcb* dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    tcb* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}
 
// Function to get front of queue
tcb* front(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
tcb* rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->rear];
}

tcb* tcb_by_id(struct Queue* queue, uint target_id) {
    if (isEmpty(queue)) {
        return NULL;
    }
    int current = queue->front;
    int max = queue->size;
    int count = 0;
    while (count < max) {
        printf("checking id %d \n", queue->array[current]->id);
        if (queue->array[current]->id == target_id) {
            printf("returning the desired node \n");
            tcb* item = queue->array[current];
            return item;
        }
        current = (queue->front + 1) % queue->capacity;
        count++;
    } 

    return NULL;
}

void print(struct Queue* queue) {
    if (isEmpty(queue)) {
        printf("queue is empty \n");
        return NULL;
    }
    int current = queue->front;
    int max = queue->size;
    int count = 0;
    while (count < max) {
        printf("%d > " , queue->array[current]->id);
        current = (queue->front + 1) % queue->capacity;
        count ++;
    }
    printf("done \n\n");
}