
#ifndef __ROUND_ROBIN__
#define __ROUND_ROBIN__

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the Task information
//----------------------------------------------------------------------------------------------------------------------------------
struct task_t {

    /// Process number for the task
    int process_id;

    // Amount of time the task takes to execute
    int execution_time;

    // Amount of time the task spends waiting to be executed
    int waiting_time;

    // Amount of time the task spends in the queue
    int turnaround_time;

	// Amount of time left for the task until it is finished
    int left_to_execute;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Intialize the task array
///
/// @param[in] task The buffer containing task data
/// @param[in] execution The execution time for each task
/// @param[in] size The size of the buffer
//----------------------------------------------------------------------------------------------------------------------------------
void init(struct task_t *task, int *execution, int size);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Run the round robin algorithm and
/// calculate the wait and turn around time for each task
///
/// @param[in] task The buffer containing task data
/// @param[in] quantum The time to allow for each task to
///                     execute between iterations
/// @param[in] size The size of the buffer
//----------------------------------------------------------------------------------------------------------------------------------
void round_robin(struct task_t *task, int quantum, int size);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Calculates the average wait time.
///
/// @param[in] task The buffer containing task data
/// @param[in] size The size of the buffer
///
/// @return The average wait time.
//----------------------------------------------------------------------------------------------------------------------------------
float calculate_average_wait_time(struct task_t *task, int size);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Calculates the average turn around time.
///
/// @param[in] task The buffer containing task data
/// @param[in] size The size of the buffer
///
/// @return The average turn around time.
//----------------------------------------------------------------------------------------------------------------------------------
float calculate_average_turn_around_time(struct task_t *task, int size);

#endif // __ROUND_ROBIN__