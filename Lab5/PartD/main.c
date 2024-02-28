#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_COLOR_OK

#include "ctest.h"

//----------------------------------------------------------------------------------------------------------------------------------
/// @Instructions
/// 1. Complete implementation of queue.c
/// 2. Complete implementation of priority.c (assume quantum time is always 1)
/// 3. Complete implementation of prioritytests.c
/// 4. This algorithm is a priority age, so once items reach a specific "aspect" they must be modified.
/// 4a. If the current tasks execution time is equal to the time in the system, multiply the priority by 4
/// 4b. If the current tasks left to execute time is equal to the time in the system, multiple the priority by 2
/// 5. Read the .h files very carefully
///
/// @Note
/// 1. You cannot modify queue.h
/// 2. You cannot modify priority.h
/// 3. You cannot modify main.c
///
/// @Makefie
/// 1. type 'make' in command line for the project to be built
/// 2. type 'make remake' to rebuild your project
/// 3. type './priority' to run your unit tests
//----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int result = ctest_main(argc, argv);

    printf("\nRan all of the tests associated with Priority Scheduling\n");
    return result;
}

