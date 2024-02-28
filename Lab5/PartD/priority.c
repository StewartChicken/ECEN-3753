#include "priority.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int *priority, int size) {

}

void priority_schedule(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array in the correct order
    // 2. Each task can be processed for a time interval of 1 (i.e quantum time of 1)
    // 3. You can process by pushing and popping items from the queue
    // 4. You must recalculate the priorities after every turn
}

float calculate_average_wait_time(struct task_t *task, int size) {
    // return 0.0 so it compiles
    return 0.0;
}

float calculate_average_turn_around_time(struct task_t *task, int size) {
    // return 0.0 so it compiles
    return 0.0;
}