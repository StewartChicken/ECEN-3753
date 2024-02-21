#include "rr.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) {
    for(int i = 0; i < size; i ++)
    {
        // Set execution time of each task
        (task + i)->execution_time = *(execution + i);
        (task + i)->left_to_execute = *(execution + i);
        (task + i)->process_id = i;

        // Wait time and turnaround time set to zero initially
        (task + i)->waiting_time = 0;
        (task + i)->turnaround_time = 0;
    }
}

void round_robin(struct task_t *task, int quantum, int size) {
    // Hints:
    // 1. Create Queue based on the task array
    // 2. Process each "task" in round robin fashion
    // 3. You can process by pushing and popping items from the queue
    
    struct node_t* task_queue = create_queue(task, size);

    int turnaround_time = 0; // Track total turnaround time

    while(!is_empty(&task_queue))
    {
        struct task_t* current_task = peek(&task_queue);
        
        // If the current task can finish its process during this cycle
        if(current_task->left_to_execute <= quantum)
        {
            // Update turnaround time
            turnaround_time += current_task->left_to_execute;
            current_task->turnaround_time = turnaround_time;

            // Update waiting time
            current_task->waiting_time = (current_task->turnaround_time 
                                            - current_task->execution_time);

            // Remove completed task from task list
            pop(&task_queue);

            continue;
        }

        // If the current task will not finish its process during this cycle

        turnaround_time += quantum;

        // Run task for specified quantum
        current_task->left_to_execute -= quantum; 
       
        push(&task_queue, current_task);                  // Push updated task to end of queue
        pop(&task_queue);                         // Pop task from front of queue  
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