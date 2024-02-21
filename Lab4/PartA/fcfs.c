#include "fcfs.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) {
    for(int i = 0; i < size; i++)
    {
        // Set execution time of each task
        (task + i)->execution_time = *(execution + i);
        (task + i)->process_id = i;

        // Wait time and turnaround time set to zero initially
        (task + i)->waiting_time = 0;
        (task + i)->turnaround_time = 0;
    }
}

void first_come_first_served(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array
    // 2. Pop off the front item until there are no more tasks

    // Creates a queue of tasks and returns the head node
    struct node_t* task_queue = create_queue(task, size); 

    int turnaround_time = 0;
    
    // Pop every element off the task queue
    for(int i = 0; i < size; i++)
    {

        struct task_t* current_task = peek(&task_queue); // Get first task
        
        current_task->waiting_time += turnaround_time; 

        // Update turnaround time
        turnaround_time += current_task->execution_time; // How long the current task takes
        current_task->turnaround_time = turnaround_time; // Calculate the turnaround time for current task

        pop(&task_queue);
    }

}

float calculate_average_wait_time(struct task_t *task, int size) {
    
    float total_waiting_time = 0.0f;

    for(int i = 0; i < size; i++)
    {
        total_waiting_time += (task + i)->waiting_time;
    }

    return total_waiting_time / ((float)size);
}

float calculate_average_turn_around_time(struct task_t *task, int size) {

    float total_turnaround_time = 0.0f;

    for(int i = 0; i < size; i++)
    {
        total_turnaround_time += (task + i)->turnaround_time;
    }

    return total_turnaround_time / ((float)size);
}