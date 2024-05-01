#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

//----------------------------------------------------------------
// Data structure declarations
//----------------------------------------------------------------

// Map config
[[maybe_unused]] typedef struct {
    uint32_t cell_count;                                // Number of cells in each row and column (map will always be a square)
    uint32_t wall_probability;                          // Probability of a single wall being created 
    uint32_t hole_probability;                          // Probability of a single hole being created
    uint8_t num_waypoints;                              // Number of waypoints to be generated
    uint8_t hole_radius;                                // The radius of each hole in pixels
    uint8_t waypoint_radius;                            // The radius of each waypoint in pixels
} MapConfig_t;

// Drone config
[[maybe_unused]] typedef struct {
    uint32_t disruptor_max_time;                        // Duration (ms) disruptor can be used for
    uint32_t disruptor_power;                           // Power draw (mW) of disruptor
    uint32_t disruptor_min_activation_energy;           // Min energy (mJ) to use disruptor 
    uint32_t max_energy;                                // Amount of energy (mJ) that can be stored
    uint32_t recharge_rate;                             // Rate of energy recharge (mW)
    uint32_t diameter;                                  // Size of drone (pixels)
    int32_t max_velocity;                               // Max velocity of drone
} DroneConfig_t;

// Physics config
[[maybe_unused]] typedef struct {
    uint32_t gravity;                                    // kg*cm / s^2
    uint32_t update_frequency;                           // Hz
    uint32_t angle_gain;                                 // 0 - 1000 (1000 means physics angle is equivalent to gyro angle)
    uint8_t pin_at_center;                               // Pin either the drone or the map at the center of the screen
} PhysicsConfig_t;

[[maybe_unused]] typedef struct {
    uint32_t time_to_complete;                           // ms        
    bool hard_edged;                                     // Can the drone leave the maze?
    bool reuse_waypoints;                                // If false, player must reach every waypoint. If true, only one non initial waypoint      
} GameConfig_t;

// Overall config
[[maybe_unused]] static struct ConfigData_t {

    uint8_t version;

    MapConfig_t map_config;
    DroneConfig_t drone_config;
    PhysicsConfig_t physics_config;
    GameConfig_t game_config;

} config;


//----------------------------------------------------------------
// Default data values
//----------------------------------------------------------------


#endif