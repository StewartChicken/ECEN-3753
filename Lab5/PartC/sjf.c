#include "sjf.h"
#include "queue.h"
#include <stdio.h>


void init(struct task_t *task, int *execution, int size) {
    
    for(int i = 0; i < size; i ++)
    {
        // Set execution time of each task
        (task + i)->execution_time = *(execution + i);
        (task + i)->process_id = i;

        // Wait time and turnaround time initialized to zero
        (task + i)->waiting_time = 0;
        (task + i)->turnaround_time = 0;
    }
}

void shortest_job_first(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array in the correct order
    // 2. Process each "task" until completion
    // 3. You can process by popping items from the queue

    // Create the task queue linked list
    struct node_t* task_queue = create_queue(task, size);

    // TODO - sort task queue by shortest execution time

    int turnaround_time = 0;

    // Pop every element off the task queue
    for(int i = 0; i < size; i ++)
    {
        struct task_t* current_task = peek(&task_queue); // Get first task

        current_task->waiting_time += turnaround_time;

        // Update turnaround time
        turnaround_time += current_task->execution_time; // How long the current task takes
        current_task ->turnaround_time = turnaround_time;

        pop(&task_queue);
    }
}

float calculate_average_wait_time(struct task_t *task, int size) {
    // return 0.0 so it compiles
    return 0.0;
}

float calculate_average_turn_around_time(struct task_t *task, int size) {
    // return 0.0 so it compiles
    return 0.0;
}