#include <stdlib.h>
#include "ctest.h"
#include "fcfs.h"

// Note: the name in the first parameter slot must match all tests in that group
CTEST_DATA(firstcomefirstserved) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(firstcomefirstserved) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, data->size);
    first_come_first_served(data->task, data->size);
}

CTEST2(firstcomefirstserved, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}


// New test cases

// Check the execution time of each task matches the expected
CTEST2(firstcomefirstserved, check_execution_time) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i + 1, (int)data->task[i].execution_time);
    }
}

// Check if the total turnaround time matches the expected
// 1 + 3 + 6 = 10
CTEST2(firstcomefirstserved, check_total_turnaround_time) {
    int total_turnaround_time = 0;

    for(int i = 0; i < data->size; i ++)
    {
        total_turnaround_time += (int)data->task[i].turnaround_time;
    }

    ASSERT_EQUAL(10, total_turnaround_time);
}

// Check if average turnaround time matches the expected (roughly)
// (1 + 3 + 6) / 3 = 10 / 3 = 3.33
// 3.33 * 100 = 333
CTEST2(firstcomefirstserved, check_average_turnaround_time) {

    float avg_time = calculate_average_turn_around_time(data->task, 3);

    avg_time *= 100; // Allows us to check equality for the returned float

    ASSERT_EQUAL(333, avg_time);
}

// Check if the total waiting time matches the expected
// 0 + 1 + 3 = 4
CTEST2(firstcomefirstserved, check_total_waiting_time) {
    int total_waiting_time = 0;

    for(int i = 0; i < data->size; i ++)
    {
        total_waiting_time += (int)data->task[i].waiting_time;
    }

    ASSERT_EQUAL(4, total_waiting_time);
}

// Check if average waiting time matches the expected (roughly)
// (0 + 1 + 3) / 3 = 4 / 3 = 1.33
// 1.33 * 100 = 133
CTEST2(firstcomefirstserved, check_average_waiting_time) {

    float avg_time = calculate_average_wait_time(data->task, 3);

    avg_time *= 100; // Allows us to check equality for the returned float

    ASSERT_EQUAL(133, avg_time);
}
