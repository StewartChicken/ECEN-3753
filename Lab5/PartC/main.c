#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_COLOR_OK

#include "ctest.h"

//----------------------------------------------------------------------------------------------------------------------------------
/// @Instructions
/// 1. Complete implementation of queue.c
/// 2. Complete implementation of sjf.c
/// 3. Complete implementation of sjftests.c
/// 4. Read the .h files very carefully
///
/// @Note
/// 1. You cannot modify queue.h
/// 2. You cannot modify sjf.h
/// 3. You cannot modify main.c
///
/// @Makefie
/// 1. type 'make' in command line for the project to be built
/// 2. type 'make remake' to rebuild your project
/// 3. type './shortestjobfirst' to run your unit tests
//----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int result = ctest_main(argc, argv);

    printf("\nRan all of the tests associated with Shortest Job First Scheduling\n");
    return result;
}

