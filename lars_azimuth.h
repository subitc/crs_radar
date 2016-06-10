/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_azimuth.h
 * Author: David
 *
 */

#ifndef LARS_AZIMUTH_H
#define LARS_AZIMUTH_H

// LARS Variables
#include "lars_variables.h"

namespace lars
{
    class azimuth
    {
    public:

        // Enum - Rotation Directions
        enum rotate_mode
        {
            ROTATE_STOP = 0,
            ROTATE_CLOCKWISE = 1,
            ROTATE_COUNTER_CLOCKWISE = -1
        };

        // Enum - Rotation Speed
        enum speed_mode
        {
            SPEED_STAGNANT = 1,
            SPEED_SLOW = 2,
            SPEED_MEDIUM = 3,
            SPEED_FAST = 4
        };

        // Constructor
        azimuth ( tty ttyDevice );

        // Destructor
        virtual ~azimuth ( );

        // Update the assigned TTY
        void update_tty ( tty ttyDevice );

        // Check if the device was initialized
        bool is_initialized ( );

        // Run Command
        int run_command ( bool expect_result, const std::string& command_string );

        // Set Manual Rotation
        bool manual_rotate ( rotate_mode mode, int stop_after_seconds = 0 );

        // Set Rotation Speed
        bool set_rotation_speed ( speed_mode mode );

        // Read Azimuth Value
        int value ( );

        // Set Azimuth Value
        bool value ( int desired_azimuth );

        // Program a List of Azimuths
        bool program_set ( int step_time, int *points );

        // Get number of points in program
        int program_points ( );

        // Start Running the Program
        bool program_start ( );

        // Stop Running the Program
        bool program_stop ( );

        // Run Offset Calibration
        bool calibrate_offset ( );

        // Run Full Calibration
        bool calibrate_full ( );

    private:

        // Private Methods
        bool initialize ( ); // Initialize the device

        // Private Serial Interface
        TimeoutSerial serial; // Serial object used to interface
        std::string dev_name; // TTY device name
        std::string dev_address; // TTY device address
        std::string dev_serialnumber; // TTY device serial number
        int dev_baud; // TTY device baud

        // Private Variables
        bool initialized; // Was the device able to initialize?
    };
}

#endif /* LARS_AZIMUTH_H */


