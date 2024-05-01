/*
 * ApplicationCode.c
 *
 *  Created on: Nov 14, 2023
 *      Author: xcowa
 */

#include "ApplicationCode.h"

/**
  * @brief Initialize application to default state - define RTOS structures
  * @param None
  * @retval None
  */
void ApplicationInit(void)
{
    LTCD__Init();
    LTCD_Layer_Init(0);
    Gyro_Init();

    // Enable RNG peripheral
    RNG_enable();

    APPLICATION_enable_button_interrupts();

    APPLICATION_configure_settings();
    APPLICATION_create_map();

    drone_energy = 15000;

    // Drone spawns on first waypoint
    drone_position_x = waypoint_data[0].x;
    drone_position_y = waypoint_data[0].y;

	[[maybe_unused]] static osStatus_t init_status;

    // Create the lcd display thread
    lcd_display_task = osThreadNew(lcd_display_task_function, (void *)0, &lcd_display_task_attributes);

    if(lcd_display_task == NULL)
    	while(1);


    // Create the disruptor thread
    disruptor_task = osThreadNew(disruptor_task_function, (void *)0, &disruptor_task_attributes);

    if(disruptor_task == NULL)
    	while(1);


    // Create the button thread
    button_task = osThreadNew(button_task_function, (void *)0, &button_task_attributes);

    if(button_task == NULL)
    	while(1);


    // Create gyro angle thread
    gyro_angle_task = osThreadNew(gyro_angle_task_function, (void *)0, &gyro_angle_task_attributes);

    if(gyro_angle_task == NULL)
        while(1);


    // Create the led output thread
    led_output_task = osThreadNew(led_output_task_function, (void *)0, &led_output_task_attributes);

    if(led_output_task == NULL)
        while(1);

    // Create game thread
    game_task = osThreadNew(game_task_function, (void *)0, &game_task_attributes);

    if(game_task == NULL)
        while(1);
        

    // Energy recharge timer initialization
    energy_recharge_timer = osTimerNew(energy_recharge_timer_callback, osTimerPeriodic, (void *)0, &energy_recharge_timer_attributes);
    
    if(energy_recharge_timer == NULL)
        while(1);

    // Energy depletion timer initialization
    energy_depletion_timer = osTimerNew(energy_depletion_timer_callback, osTimerPeriodic, (void *)0, &energy_depletion_timer_attributes);
    
    if(energy_depletion_timer == NULL)
        while(1);

    // =================================================================================================
    /* Green led pwm timer initialization */
    //
    green_led_pwm_timer = osTimerNew(green_led_pwm_timer_callback, osTimerPeriodic, (void *)0, &green_led_pwm_timer_attributes);

    if(green_led_pwm_timer == NULL)
        while(1);

    init_status = osTimerStart(green_led_pwm_timer, 1U);

    // =================================================================================================
    /* Red led timer initialization */
    //
    red_led_timer = osTimerNew(red_led_timer_callback, osTimerPeriodic, (void *)0, &red_led_timer_attributes);

    if(red_led_timer == NULL)
        while(1);

    // =================================================================================================
    /* Game timer initialization */
    //
    game_timer = osTimerNew(game_timer_callback, osTimerPeriodic, (void *)0, &game_timer_attributes);

    if(game_timer == NULL)
        while(1);

    // =================================================================================================
    /* Gyro angle data mutex initialization */
    //
    gyro_angle_mutex = osMutexNew(&gyro_angle_mutex_attributes);

    if(gyro_angle_mutex == NULL)
        while(1);

    
    // =================================================================================================
    /* Drone position data mutex initialization */
    //
    drone_position_mutex = osMutexNew(&drone_position_mutex_attributes);

    if(drone_position_mutex == NULL)
        while(1);


    // =================================================================================================
    /* Button event initialization */
    //
    energy_event = osEventFlagsNew(&energy_event_attributes);

    if(energy_event == NULL)
        while(1);


    // =================================================================================================
    /* Led event flag group initialization */
    //
    led_event = osEventFlagsNew(&led_event_attributes);

    if(led_event == NULL)
        while(1);


    // Button semaphore
    button_semaphore = osSemaphoreNew(1, 0, &button_semaphore_attributes);

    if(button_semaphore == NULL)
        while(1);


    // Start game tick timer
    init_status = osTimerStart(game_timer, 1U);
}

/**
  * @brief 
  * @param 
  * @retval 
  */
void APPLICATION_configure_settings(void)
{

    config.version = 1;

    // Game config
    config.game_config.time_to_complete = 30000; // 30 sec
    config.game_config.hard_edged = true;
    config.game_config.reuse_waypoints = false; // All waypoints must be reached

    // Map config
    config.map_config.cell_count = 6;
    config.map_config.wall_probability = 150;  // - 150 / 1000 = 15 %
    config.map_config.hole_probability = 150;  // - 150 / 1000 = 15 %
    config.map_config.num_waypoints = 4; 
    config.map_config.hole_radius = 10;        // Pixels
    config.map_config.waypoint_radius = 15;    // Pixels

    // Drone config
    config.drone_config.disruptor_max_time = 1000;                        
    config.drone_config.disruptor_power = 10000;                           
    config.drone_config.disruptor_min_activation_energy = 6000;           
    config.drone_config.max_energy = 15000;                                
    config.drone_config.recharge_rate = 1000;                             
    config.drone_config.diameter = 10;
    config.drone_config.max_velocity = 2500;

    // Physics config
    config.physics_config.gravity = 980;
    config.physics_config.update_frequency = 50;
    config.physics_config.angle_gain = 500;
    config.physics_config.pin_at_center = MAZE; // This does not actually affect the configuration in this version
}


/**
  * @brief Enable button interrupts in NVIC - set priority to default priority
  * @retval None
  */
void APPLICATION_enable_button_interrupts(void)
{
        HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
        HAL_NVIC_SetPriority(BUTTON_IRQ_NUMBER, EXTI0_DEFAULT_PRIORITY, EXTI0_DEFAULT_PRIORITY);
}

/**
  * @brief Read button pin and update application value
  * @retval None
  */
void APPLICATION_sample_button(void)
{
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        button_state = GPIO_PIN_SET;
    }
    else{
        button_state = GPIO_PIN_RESET;
    }
}

/**
  * @brief Sample gyroscope and record rotation rate in gyro_rotation variable
  * @retval None
  */
void APPLICATION_sample_gyro(void)
{
    int16_t gyro_velocity_x = Gyro_Get_Velocity_X(); 
    int16_t gyro_velocity_y = Gyro_Get_Velocity_Y();

    // Obtain angular velocity in mdps
    double angular_velocity_x = ((double)gyro_velocity_x) * 0.07; 
    double angular_velocity_y = ((double)gyro_velocity_y) * 0.07;
    
    gyro_angle_x += (int)(angular_velocity_x * 0.01);
    gyro_angle_y += (int)(angular_velocity_y * 0.01);
}

/**
  * @brief Turn on green led
  * @retval None
  */
void APPLICATION_activate_green_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}

/**
  * @brief Turn on red led
  * @retval None
  */
void APPLICATION_activate_red_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
}

/**
  * @brief Turn off green led
  * @retval None
  */
void APPLICATION_deactivate_green_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

/**
  * @brief Turn off red led
  * @retval None
  */
void APPLICATION_deactivate_red_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

/**
  * @brief Toggle green led
  * @retval None
  */
void APPLICATION_toggle_green_led(void)
{
    if(green_led_state == 1)
    {
        green_led_state = 0;
        APPLICATION_deactivate_green_led();
    } else {
        green_led_state = 1;
        APPLICATION_activate_green_led();
    }
}

/**
  * @brief Toggle red led
  * @retval None
  */
void APPLICATION_toggle_red_led(void)
{
    if(red_led_state == 1)
    {
        red_led_state = 0;
        APPLICATION_deactivate_red_led();
    } else {
        red_led_state = 1;
        APPLICATION_activate_red_led();
    }
}


/**
 * @brief Creates the initial map structure - determines where all the walls, waypoints and holes are.
 *        It loads these values into the global map data structure to be read by other threads. This also
 *         initializes the drone position to default (center of the map)
 * 
 * @param void
 * @return void
 */
void APPLICATION_create_map(void)
{
    uint32_t rand;
    num_holes = 0;

    uint8_t num_waypoints = config.map_config.num_waypoints;
    uint8_t increment;


    // Create the map array

    // For every cell in the map
    for(int i = 0; i < config.map_config.cell_count; i++)
    {
        for(int j = 0; j < config.map_config.cell_count; j ++)
        {
            cell_data[i][j] = 0; // Initialize to 0

            // Get random number
            rand = RNG_get_random_number(1000); 

            // Generate bottom wall
            if(rand < config.map_config.wall_probability)           
                cell_data[i][j] |= 0b00000010;

            rand = RNG_get_random_number(1000); 

            // Generate right wall
            if(rand < config.map_config.wall_probability)           
                cell_data[i][j] |= 0b00001000;

            rand = RNG_get_random_number(1000); 

            // Generate hole
            if(rand < config.map_config.hole_probability)
            {
                cell_data[i][j] |= 0b00010000;
                num_holes ++;
            }            
                
        }
    }

    // Waypoint generation

    increment = 0;

    while(increment < num_waypoints)
    {
        uint32_t randRow = RNG_get_random_number(config.map_config.cell_count);
        uint32_t randCol = RNG_get_random_number(config.map_config.cell_count);

        // If a waypoint has already been generated in this cell
        if(cell_data[randRow][randCol] & 0x20)
            continue;

        // If a hole has been generated in this cell, override it with a waypoint
        if(cell_data[randRow][randCol] & 0x10)
        {
            cell_data[randRow][randCol] &= 0b11101111; // Clear hole data
            num_holes --;
        }

        // If the cell is empty, generate a new waypoint
        cell_data[randRow][randCol] |= 0x20; // Set waypoint data
        waypoint_data[increment].x = (randCol * 40) + 20;
        waypoint_data[increment].y = (randRow * 40) + 60;
        waypoint_data[increment].number = increment;
        waypoint_data[increment].reached = false;        

        increment ++;
    }

    // Create arrays of hole data
    hole_data = (HoleData_t *)calloc(num_holes, sizeof(HoleData_t));
    if(hole_data == NULL)
        while(1);
    
    // Use increment to index the generated holes
    increment = 0;

    // Get hole and waypoint location data 
    for(int i = 0; i < config.map_config.cell_count; i ++)
    {
        for(int j = 0; j < config.map_config.cell_count; j ++)
        {   
            // If a hole is generated within this cell, place its center coordinate within the holes array
            if(cell_data[i][j] & 0x10)
            {
                hole_data[increment].x = (j * 40) + 20;
                hole_data[increment].y = (i * 40) + 60;

                increment ++;
            }
        }
    }
}

/**
 * @brief Calls LCD functions to draw the current map
 * 
 * @param void
 * @return void
 */
void APPLICATION_draw_map(void)
{
    // Map boundaries
    LCD_Draw_Line(0, 40, 239, 40, LCD_COLOR_BLACK);         // Top
    LCD_Draw_Line(0, 280, 239, 280, LCD_COLOR_BLACK);       // Bottom
    LCD_Draw_Line(0, 40, 0, 280, LCD_COLOR_BLACK);          // Left
    LCD_Draw_Line(239, 40, 239, 280, LCD_COLOR_BLACK);      // Right

    // Loop through matrix of map data and draw map accordingly
    for(int i = 0; i < config.map_config.cell_count; i++)
    {
        for(int j = 0; j < config.map_config.cell_count; j ++)
        {
            // Top Line
            if(cell_data[i][j] & 0x1)
                LCD_Draw_Line(0 + (40 * j), 40 + (40 * i), 40 + (40 * j), 40 + (40 * i), LCD_COLOR_BLACK);  
                
            // Bottom line
            if(cell_data[i][j] & 0x2)
                LCD_Draw_Line(0 + (40 * j), 80 + (40 * i), 40 + (40 * j), 80 + (40 * i), LCD_COLOR_BLACK);  
                
            // Left line
            if(cell_data[i][j] & 0x4)
                LCD_Draw_Line(0 + (40 * j), 40 + (40 * i), 0 + (40 * j), 80 + (40 * i), LCD_COLOR_BLACK);   

            // Right line
            if(cell_data[i][j] & 0x8)
                LCD_Draw_Line(40 + (40 * j), 40 + (40 * i), 40 + (40 * j), 80 + (40 * i), LCD_COLOR_BLACK); 

            // Hole
            if(cell_data[i][j] & 0x10)
                LCD_Draw_Circle_Fill(20 + (40 * j), 60 + (40 * i), config.map_config.hole_radius, LCD_COLOR_BLACK);

            // Waypoints
            if(cell_data[i][j] & 0x20)
            {
                // Check which waypoint to draw
                for(int k = 0; k < config.map_config.num_waypoints; k ++)
                {
                    // If x and y coordinates match
                    if(waypoint_data[k].x == (20 + (40 * j)) && waypoint_data[k].y == (60 + (40 * i)))
                    {
                        // Waypoints are green if they've been reached previously
                        // Otherwise, they are red
                        if(waypoint_data[k].reached)
                        {
                            LCD_Draw_Circle_Fill(20 + (40 * j), 60 + (40 * i), config.map_config.waypoint_radius, LCD_COLOR_GREEN);
                        }
                        else {
                            LCD_Draw_Circle_Fill(20 + (40 * j), 60 + (40 * i), config.map_config.waypoint_radius, LCD_COLOR_RED);
                        }
                        
                        // Display the waypoints number at its approximate center
                        LCD_DisplayNumber(17 + (40 * j), 56 + (40 * i), waypoint_data[k].number);
                    }
                }
                
            }
              
        }
    }
}

/**
  * @brief Depending on the angle of the board, figures out the adjusted force due to gravity
  * @param int16_t - angle of board
  * @retval int32_t - adjusted gravitational force
  */
int32_t APPLICATION_get_board_gravity_ratio(int16_t angle)
{
        int8_t mult = 1;
        int32_t accel;

        angle -= 180;
        
        if(angle < 0)
        {
            mult *= -1;
            angle *= -1;
        }

        angle = (angle * config.physics_config.angle_gain) / 1000;

        if(angle > 70)
        {
            exceeded_tilt = true;
            game_lost = true;
        }
        
        // Approximate sine of board angle 
        if(angle <= 5)
        {
            accel = 0; 
        } 
        else if(angle <= 10)
        {
            accel = 15; 
        }
        else if(angle <= 20)
        {
            accel = 30; 
        }
        else if(angle <= 40)
        {
            accel = 50;
        }
        else 
        {
            accel = 75;
        }

        // Get acceleration ratio with gravity
        accel *= (mult * config.physics_config.gravity);
        accel /= 100;

        return accel;
}


/**
  * @brief Check if specified x and y coordinates are over a hole in the map
  * @param int32_t xCoor, yCoor - coordinates to check
  * @retval bool - True if coordinates lie over hole, false otherwise
  */
bool APPLICATION_is_over_hole(int32_t xCoor, int32_t yCoor)
{
    int32_t x_distance;
    int32_t y_distance;
    double distance;

    for(int i = 0; i < num_holes; i ++)
    {
        // Distance formula for distance between coordinate and a hole

        x_distance = xCoor - hole_data[i].x;
        x_distance *= x_distance;

        y_distance = yCoor - hole_data[i].y;
        y_distance *= y_distance;

        distance = sqrt(x_distance + y_distance);

        if(distance < config.map_config.hole_radius)
            return true;
    }

    return false;
}

/**
  * @brief Determines if a point is over a waypoint and, if so, which waypoint
  * @param int32_t xCoor, yCoor - Coordiante to check 
  * @retval int8_t - -1 if not over waypoint, otherwise returns the waypoint number
  */
int8_t APPLICATION_is_over_waypoint(int32_t xCoor, int32_t yCoor)
{
    int32_t x_distance;
    int32_t y_distance;
    double distance;

    for(int i = 0; i < config.map_config.num_waypoints; i ++)
    {
        // Distance formula for distance between coordinate and a waypoint

        x_distance = xCoor - waypoint_data[i].x;
        x_distance *= x_distance;

        y_distance = yCoor - waypoint_data[i].y;
        y_distance *= y_distance;

        distance = sqrt(x_distance + y_distance);

        if(distance < config.map_config.waypoint_radius)
            return waypoint_data[i].number;
    }

    return -1;
}

/**
  * @brief Check if the xCoor and yCoor are over a horizontal wall
  * @param int32_t xCoor, yCoor - coordinates to check
  * @retval bool - true if collision with wall is detected
  */
bool APPLICATION_check_horizontal_wall_collision(int32_t xCoor, int32_t yCoor)
{
    // Generate index from xCoor and yCoor
    // xCoor and yCoor can be used to figure out which cell the drone is in as well as the neighboring cells
    // These cells can then be checked to see if a part of the drone is lying within a wall
    // 5 cells in total will be checked

    // 6 x 6 cells - 40 pixels each

    // Obtain the cell row and column information from the x and y coordinates of the drone's center
    cellRow = (yCoor - 40) / 40;
    cellCol = xCoor / 40;

    // If cell right wall is collided with
    if(cell_data[cellRow][cellCol] & 0x8)
    {
        if(APPLICATION_check_drone_overlap(xCoor, cellCol * 40 + 40))
            return true;
    }    

    // Check if cell left wall is collided with
    if(cellCol != 0)
    {   
        if(cell_data[cellRow][cellCol - 1] & 0x8)
        {
            if(APPLICATION_check_drone_overlap(xCoor, (cellCol - 1) * 40 + 40))
                return true;
        }   
    }
     
    return false;
}

/**
  * @brief Check if the coordinates overlap with a vertical wall
  * @param int32_t xCoor, yCoor - coordinate to check
  * @retval bool - True if overlapping with vertical wall, false otherwise
  */
bool APPLICATION_check_vertical_wall_collision(int32_t xCoor, int32_t yCoor)
{
    // Obtain the cell row and column information from the x and y coordinates of the drone's center
    cellRow = (yCoor - 40) / 40;
    cellCol = xCoor / 40;

    // If cell bottom wall is collided with
    if(cell_data[cellRow][cellCol] & 0x2)
    {
        if(APPLICATION_check_drone_overlap(yCoor, cellRow * 40 + 80))
            return true;
    }

    if(cellRow != 0)
    {
        if(cell_data[cellRow - 1][cellCol] & 0x2)
        {
            if(APPLICATION_check_drone_overlap(yCoor, (cellRow - 1) * 40 + 80))
                return true;
        }
    }

    return false;
}

/**
  * @brief Check overlap with a horizontal or vertical line
  * @param uint32_t drone_pos, line_pos - the two locations to check for overlap
  * @retval bool - true if overlap, false otherwise
  */
bool APPLICATION_check_drone_overlap(int32_t drone_pos, int32_t line_pos)
{
    if(abs(drone_pos - line_pos) <= config.drone_config.diameter / 2)
        return true;

    return false;
}

/**
  * @brief Function for the LCD thread - updates every frame
  * @param void *arg - pointer to argument array
  * @retval None
  */
void lcd_display_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

	while(1)
	{
        if(game_won)
        {   
            LCD_Clear(0, LCD_COLOR_WHITE);
            LCD_SetTextColor(LCD_COLOR_GREEN);
            LCD_SetFont(&Font16x24);

            LCD_DisplayString(40, 148, "You Win!!!");

            osDelay(LCD_UPDATE_RATE);
            continue;
        }

        if(game_lost)
        {
            LCD_Clear(0, LCD_COLOR_WHITE);
            LCD_SetTextColor(LCD_COLOR_RED);
            LCD_SetFont(&Font16x24);

            LCD_DisplayString(45, 148, "You Lost!!");

            LCD_SetFont(&Font12x12);

            if(fell_into_hole)
            {
                LCD_DisplayString(10, 175, "Drone was lost!");
            } 
            else if(ran_out_of_time)
            {
                LCD_DisplayString(35, 175, "Out of time!");
            }
            else if(exceeded_tilt)
            {
                LCD_DisplayString(15, 175, "Drone fell off");
                LCD_DisplayString(75, 190, "Board!");
            }

            osDelay(LCD_UPDATE_RATE);
            continue;
        }

        LCD_Clear(0, LCD_COLOR_WHITE);
        LCD_SetTextColor(LCD_COLOR_BLACK);
	    LCD_SetFont(&Font12x12);

        APPLICATION_draw_map();

        status = osMutexAcquire(drone_position_mutex, osWaitForever);
        LCD_Draw_Circle_Fill(drone_position_x, drone_position_y, config.drone_config.diameter / 2, LCD_COLOR_BLUE);
        status = osMutexRelease(drone_position_mutex);

        // Display disruptor energy level
        LCD_DisplayString(10, 300, "Energy: ");
        LCD_DisplayNumber(110, 300, drone_energy);

        // Display time remaining
        LCD_DisplayString(10, 15, "Time: ");
        LCD_DisplayNumber(92, 15, (config.game_config.time_to_complete - game_tick) / 1000 + 1);

		osDelay(LCD_UPDATE_RATE);
	}
}

/**
  * @brief Function for disruptor thread - controls energy level of disruptor as well as its usability
  * @param void *arg - pointer to argument array
  * @retval None
  */
void disruptor_task_function(void *arg)
{
	(void) &arg; // Remove warnings
	[[maybe_unused]] uint32_t flags;
	[[maybe_unused]] osStatus_t status;

	while(1)
	{	
        if(game_won || game_lost)
        {
            status = osThreadYield();
        }

		flags = osEventFlagsWait(energy_event, 0x0003, osFlagsWaitAny, osWaitForever); // Wait for button event to occur
 
        if(flags & DEPLETE_ENERGY_EVENT)
        {
            
            drone_energy -= config.drone_config.disruptor_power / 100;
            if(drone_energy <= 0)
            {
                drone_energy = 0;
                osTimerStop(energy_depletion_timer);
            }

            // Energy level below minimum activation 
            if(drone_energy < config.drone_config.disruptor_min_activation_energy)
            {
                disruptor_can_be_activated = 0;
                status = osTimerStart(red_led_timer, 1U);
            }
        }

        if(flags & RECHARGE_ENERGY_EVENT)
        {
            drone_energy += config.drone_config.recharge_rate / 100;

            if(drone_energy >= config.drone_config.max_energy)
            {
                drone_energy = config.drone_config.max_energy;
                osTimerStop(energy_recharge_timer);
            }

            // Energy satisfies minimum activation
            if(drone_energy >= config.drone_config.disruptor_min_activation_energy)
            {
                disruptor_can_be_activated = 1;
                status = osTimerStop(red_led_timer);
                flags = osEventFlagsSet(led_event, DISABLE_RED_LED_EVENT); // Disable red led
            }
        }
	}
}

/**
  * @brief Function for led thread - controls led outputs
  * @param void *arg - pointer to argument array
  * @retval None
  */
void led_output_task_function(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;
    uint32_t event_flags;

    while(1)
    {
        if(game_won || game_lost)
        {
            APPLICATION_deactivate_red_led();
            APPLICATION_deactivate_green_led();
            status = osThreadYield();
        }
        
        // Wait for an LED event 
        event_flags = osEventFlagsWait(led_event, 0x000F, osFlagsWaitAny, osWaitForever);

        if(event_flags & ENABLE_RED_LED_EVENT)
        {
            APPLICATION_activate_red_led();
        } 
        else if(event_flags & DISABLE_RED_LED_EVENT)
        {
            APPLICATION_deactivate_red_led();
        }


        if(event_flags & ENABLE_GREEN_LED_EVENT)
        {
            APPLICATION_activate_green_led();
        }
        else if(event_flags & DISABLE_GREEN_LED_EVENT)
        {
            APPLICATION_deactivate_green_led();
        }
    }
}

/**
  * @brief Function for button thread - Activates/deactivates disruptor
  * @param void *arg - pointer to argument array
  * @retval None
  */
void button_task_function(void *arg)
{
	(void) &arg; // Remove warnings
	[[maybe_unused]] osStatus_t status;
    
	while(1)
	{
        if(game_won || game_lost)
        {
            status = osThreadYield();
        }

		status = osSemaphoreAcquire(button_semaphore, osWaitForever); // Wait for button press

		if(button_state == BUTTON_PRESSED)
        {
            // Start depleting energy level while button is pressed
            status = osTimerStop(energy_recharge_timer);
            status = osTimerStart(energy_depletion_timer, 10);

            if(disruptor_can_be_activated)
            {
                disruptor_active = true;
            }
            else {
                disruptor_active = false;
            }
        } 
        else if(button_state == BUTTON_NOT_PRESSED)
        {
            status = osTimerStop(energy_depletion_timer);
            status = osTimerStart(energy_recharge_timer, 10);

            disruptor_active = false;
        }
	}

}

/**
  * @brief Function for the gyroscope thread - Updates values of gyroscope
  * @param void *arg - pointer to argument array
  * @retval None
  */
void gyro_angle_task_function(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

    while(1)
    {
        if(game_won || game_lost)
        {
            status = osThreadYield();
        }

        status = osMutexAcquire(gyro_angle_mutex, osWaitForever);
        APPLICATION_sample_gyro();
        status = osMutexRelease(gyro_angle_mutex);

        osDelay(GYRO_SAMPLE_RATE);
    }
}

/**
  * @brief Main game thread - controls drone position, game collision, and physics updates
  * @param void *arg - pointer to argument array
  * @retval None
  */
void game_task_function(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

    [[maybe_unused]] int16_t board_angle_x, board_angle_y; // Angle of the board itself
    [[maybe_unused]] int16_t accel_x, accel_y; // Axial acceleration for the drone
   
    while(1)
    {
        if(game_won || game_lost)
        {
            status = osTimerStop(green_led_pwm_timer);
            status = osTimerStop(red_led_timer);
            status = osTimerStop(energy_recharge_timer);
            status = osTimerStop(energy_depletion_timer);

            status = osThreadYield();
        }

        status = osMutexAcquire(gyro_angle_mutex, osWaitForever);
        board_angle_x = gyro_angle_x;
        board_angle_y = gyro_angle_y;
        status = osMutexRelease(gyro_angle_mutex);

        accel_x = APPLICATION_get_board_gravity_ratio(board_angle_x);
        accel_y = APPLICATION_get_board_gravity_ratio(board_angle_y);

        status = osMutexAcquire(drone_position_mutex, osWaitForever);

        // Update drone velocity
        drone_velocity_x += accel_x;
        drone_velocity_y += accel_y;

        // Limit drone velocity
        
        if(drone_velocity_x > config.drone_config.max_velocity)
            drone_velocity_x = config.drone_config.max_velocity;

        if(drone_velocity_x < -config.drone_config.max_velocity)
            drone_velocity_x = -config.drone_config.max_velocity;

        if(drone_velocity_y > config.drone_config.max_velocity)
            drone_velocity_y = config.drone_config.max_velocity;

        if(drone_velocity_y < -config.drone_config.max_velocity)
            drone_velocity_y = -config.drone_config.max_velocity;

        // Check if drone collides with right map boundary
        if(drone_position_x + (drone_velocity_y / 1000) > 238 - config.drone_config.diameter / 2)
        {
            drone_velocity_y = 0;
        }

        // Check if drone collides with left map boundary
        if(drone_position_x + (drone_velocity_y / 1000) < 0 + config.drone_config.diameter / 2)
        {
            drone_velocity_y = 0;
        }

        // Check if drone collides with top map boundary
        if(drone_position_y + (drone_velocity_x / 1000) < 40 + config.drone_config.diameter / 2)
        {
            drone_velocity_x = 0;
        }

        // Check if drone collides with bottom map boundary
        if(drone_position_y + (drone_velocity_x / 1000) > 280 - config.drone_config.diameter / 2)
        {
            drone_velocity_x = 0;
        }

        // Check if drone collides with wall on x axis
        if(APPLICATION_check_horizontal_wall_collision(drone_position_x + drone_velocity_y / 1000, drone_position_y))
        {
            if(disruptor_active)
            {
                drone_position_x += drone_velocity_y / 300;
            }
            else {
                drone_velocity_y = 0;
            }
        } 
        
        // Check if drone collides with wall on y axis
        if(APPLICATION_check_vertical_wall_collision(drone_position_x, drone_position_y + drone_velocity_x / 1000))
        {
            if(disruptor_active)
            {
                drone_position_y += drone_velocity_x / 300;
            }
            else {
                drone_velocity_x = 0;
            }
        }

        // Update drone position on x axis
        drone_position_x += drone_velocity_y / 1000;

        // Update drone position on y axis
        drone_position_y += drone_velocity_x / 1000;

        // Check which waypoint the drone is over (if it is over any at all)
        int8_t waypoint_number = APPLICATION_is_over_waypoint(drone_position_x, drone_position_y);

        // If the drone *is* over a waypoint and it is the right waypoint
        if(waypoint_number != -1 && waypoint_number == current_waypoint)
        {
            // That waypoint has been reached by the drone
            waypoint_data[waypoint_number].reached = true;
            current_waypoint ++;
        }

        // If all waypoints have been reached
        if(current_waypoint == config.map_config.num_waypoints)
        {
            game_won = true;
        }

        // Lost game if over hole and the disruptor is not active
        if(APPLICATION_is_over_hole(drone_position_x, drone_position_y) && disruptor_active == false)
        {
            fell_into_hole = true;
            game_lost = true;
        }
        
        status = osMutexRelease(drone_position_mutex);

        osDelay(config.physics_config.update_frequency);
    }
}

/**
  * @brief Periodically recharges disruptor energy
  * @param void *arg - pointer to argument array
  * @retval None
  */
void energy_recharge_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;
    
    event_flags = osEventFlagsSet(energy_event, RECHARGE_ENERGY_EVENT);
}

/**
  * @brief Periodically deminishes disruptor energy
  * @param void *arg - pointer to argument array
  * @retval None
  */
void energy_depletion_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;
    
    event_flags = osEventFlagsSet(energy_event, DEPLETE_ENERGY_EVENT);
}

/**
  * @brief Controls the green LED's pwm implementation
  * @param void *arg - pointer to argument array
  * @retval None
  */
void green_led_pwm_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;

    if(green_led_timer_tick < (int)((drone_energy * 10) / config.drone_config.max_energy))
    {
        event_flags = osEventFlagsSet(led_event, ENABLE_GREEN_LED_EVENT); // Enable green led
    } 
    else
    {
        event_flags = osEventFlagsSet(led_event, DISABLE_GREEN_LED_EVENT); // Disable green led
    } 

    green_led_timer_tick ++; // Increment timer tick

    // Reset cycle
    if(green_led_timer_tick == 10)
    {
        green_led_timer_tick = 0;
    }
}

/**
  * @brief Controls red led flash rate
  * @param void *arg - pointer to argument array
  * @retval None
  */
void red_led_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;

    red_led_timer_period = (config.drone_config.disruptor_min_activation_energy - drone_energy) / 10; // Number of ticks (ms)

    // Turn red led on for half a period, off for the other half
    if(red_led_timer_tick < red_led_timer_period / 2)
    {
        event_flags = osEventFlagsSet(led_event, ENABLE_RED_LED_EVENT);
    } 
    else if(red_led_timer_tick >= red_led_timer_period / 2)
    {
        event_flags = osEventFlagsSet(led_event, DISABLE_RED_LED_EVENT);
    }

    red_led_timer_tick ++; // Increment timer tick

    // Reset tick if needed
    if(red_led_timer_tick >= red_led_timer_period)
    {
        red_led_timer_tick = 0;
    }

}

/**
  * @brief Updates game tick 
  * @param void *arg - pointer to argument array
  * @retval None
  */
void game_timer_callback(void *args)
{
    (void) &args; // Remove warnings
    game_tick ++;

    if(game_tick >= config.game_config.time_to_complete)
    {
        ran_out_of_time = true;
        game_lost = true;
    }
}

/**
  * @brief EXTI0 interrupt handler - 
  * 
  * @retval None
  */
void EXTI0_IRQHandler()
{
    HAL_NVIC_DisableIRQ(BUTTON_IRQ_NUMBER);

    [[maybe_unused]] osStatus_t status;

    // Update button state by reading external button pin
    APPLICATION_sample_button();

    // Release semaphore token
    status = osSemaphoreRelease(button_semaphore);
    
    // Clear button's pending interrupt
    EXTI_HandleTypeDef button;
    button.Line = 0;
    HAL_EXTI_ClearPending(&button, EXTI_TRIGGER_RISING_FALLING);

    HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
}
