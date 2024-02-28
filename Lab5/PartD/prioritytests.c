#include <stdlib.h>
#include "ctest.h"
#include "priority.h"

CTEST_DATA(priority) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(priority) {
    int execution[] = {1, 2, 3};
    int priority[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, priority, data->size);
    priority_schedule(data->task, data->size);
}

CTEST2(priority, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

// TODO add additional tests to help debug
