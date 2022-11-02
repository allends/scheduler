#include "queue.h"

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue()
{
    struct Queue* queue = (struct Queue*)malloc(
        sizeof(struct Queue));
    queue->size = 0;
    Node* head = malloc(sizeof(Node));
    Node* tail = malloc(sizeof(Node));
    head->next = (struct Node*) tail;
    head->value = NULL;
    tail->value = NULL;
    tail->next = NULL;
    queue->head = head;
    queue->tail = tail;
    return queue;
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

// Return the size of the eueue
int queueSize(struct Queue* queue)
{
    return (queue->size);
}
 
// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, tcb* item)
{
    if (queue->head->value == NULL) {
        queue->head->value = item;
        queue->size = queue->size + 1;
        return;
    }
    if (queue->tail->value == NULL) {
        queue->tail->value = item;
        queue->size = queue->size + 1;
        return;
    }
    Node* new_tail = malloc(sizeof(Node));
    new_tail->value = item;
    queue->tail->next = (struct Node*) new_tail;
    queue->tail = new_tail;
    queue->size = queue->size + 1;
}
 
// Function to remove an item from queue.
// It changes front and size
tcb* dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    if (queue->size == 1) {
        tcb* item = queue->head->value;
        queue->size = queue->size - 1;
        queue->head->value = NULL;
        return item;
    }
    if (queue->size == 2) {
        tcb* item = queue->head->value;
        queue->size = queue->size - 1;
        queue->head->value = queue->tail->value;
        queue->tail->value = NULL;
        return item;
    }
    Node* item = queue->head;
    queue->head = (Node*) queue->head->next;
    queue->size = queue->size - 1;
    tcb* val = item->value; 
    free(item); 
    return val;
}
 
// Function to get front of queue
Node* front(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->head;
}
 
// Function to get rear of queue
Node* rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->tail;
}

tcb* tcb_by_id(struct Queue* queue, unsigned int target_id) {
    if (isEmpty(queue)) {
        return NULL;
    }
    Node* current = queue->head;
    // int max = queue->size;
    // int count = 0;
    while (current != NULL) {
        if (current->value && current->value->id == target_id) {
            return current->value;
        }
        current = (Node*) current->next;
    } 
    return NULL;
}

void remove_by_id(struct Queue* queue, unsigned int target_id) {
    if (isEmpty(queue)) {
        return;
    }
    Node* current = queue->head;
    Node* previous = NULL;
    int max = queue->size;
    int count = 0;
    while (current->value->id != target_id && current->next != NULL ) {
        previous = current;
        current = (Node*) current->next;
    } 
    if (current->value->id == target_id) {
        if (queue->size == 1) {
            queue->head->value = NULL;
        }
        else if (queue->size == 2) {
            if(previous == NULL){
                queue->head->value = queue->tail->value;
                queue->tail->value = NULL;
            }
            else if(current->next == NULL){
                queue->head->next = NULL;
                queue->tail->value = NULL;
            }
        }
        else{
            if(current == queue->head){
                queue->head = (Node*) current->next; 
            }
            else{
                previous->next = current->next;
            }
            free(current); 
        }
        queue->size = queue->size - 1;
    }
}

int all_done(struct Queue* queue) {
    if(isEmpty(queue)) {
        return 1;
    }
    Node* current = front(queue);
    while (current && current->value) {
        if (current->value->status != DONE) {
            return 0;
        }
        current = (Node*) current->next;
    }
    return 1;
}

void destroy_queue(struct Queue* queue) {
   if(isEmpty(queue)) {
        return;
    }
    Node* current = front(queue);
    while (current && current->value) {
        free(current->value);
        current = (Node*) current->next;
    }
    return; 
}

void print(struct Queue* queue) {
    if (isEmpty(queue)) {
        printf("\nqueue is empty!! \n");
        return;
    }
    Node* current = queue->head;
    int max = queue->size;
    while (current->value != NULL) {
        printf("%d > " , current->value->id);
        if (current->next == NULL) {break;}
        current = (Node*) current->next;
    }
    printf("done \n\n");
}