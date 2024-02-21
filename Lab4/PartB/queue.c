#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// struct task_t* task is a pointer to the head of an array of tasks
// Size is its current size
struct node_t* create_queue(struct task_t* task, int size) {
    
    if(size <= 0)
        return NULL; // Invalid queue size

    // Create the head node using the first task in the task array
    struct node_t* head_node = create_new_node(task);

    // This is a circular queue. The head node will point to itself initially
    head_node->next = head_node;

    struct node_t* temp_node = head_node;

    for(int i = 1; i < size ; i ++)
    {
        temp_node->next = create_new_node(task + i);
        temp_node = temp_node->next;

        temp_node->next = head_node; // Point tail node to head node
    }

    return head_node;
}

// Helper function to create new node
struct node_t* create_new_node(struct task_t* task) {
    
    // Dynamically allocate a single node within memory - initialize to zero
    struct node_t* node = (struct node_t*) calloc(1, sizeof(struct node_t));

    // Set task of new node
    node->task = task;

    // Check success of memory allocation
    if(node == NULL)
        return NULL;

    return node;
}

// This function returns the task data of the head node of the structure
struct task_t* peek(struct node_t** head) {
    
    // Head pointer is null - queue is empty
    if(head == NULL || *head == NULL)
        return NULL;

    // Return task of head pointer
    return (*head)->task;
}

// Dequeue
void pop(struct node_t** head) {

    // Head pointer is null - queue is empty
    if(head == NULL || *head == NULL)
        return;

    // Store the pointer to the head node in a temporary variable
    struct node_t* temp_node = *head; 

    while(temp_node->next != *head){
        temp_node = temp_node->next;
    }

    // Check if the queue only has one element
    if(temp_node == *head)
    {
        free(temp_node);
        temp_node = NULL;
        *head = NULL;
        return;
    }

    // Tail of queue points to new head
    temp_node->next = (*head)->next; 

    temp_node = *head;
    *head = (*head)->next;

    free(temp_node); // Free old head
    temp_node = NULL;
}

// Enqueue
void push(struct node_t** head, struct task_t* task) {

    struct node_t* temp_node = *head;

    // Find end of queue
    while(temp_node->next != *head)
    {
        temp_node = temp_node->next;
    }

    temp_node->next = create_new_node(task);
    temp_node = temp_node->next;
    temp_node->next = *head;
}

int is_empty(struct node_t** head) {
    
    // Return 1 if head node is null (queue is empty)
    if(*head == NULL)
        return 1;

    return 0;
}

void empty_queue(struct node_t** head) {

    // Head pointer is null - queue is already empty
    if(head == NULL || *head == NULL)
    {
        free(*head);
        return;
    }

    struct node_t** temp_node = head;

    while(*temp_node != NULL)
    {
        pop(temp_node);
    }
}
