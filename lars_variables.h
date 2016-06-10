/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_variables.h
 * Author: David
 *
 * Created on March 30, 2016, 1:27 PM
 */

#ifndef LARS_VARIABLES_H
#define LARS_VARIABLES_H

// Include string
#include <string>
#include <iostream>
#include <locale>
#include <complex>
#include <ctime>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <vector>

// Import USB Device Controller
#include <linux/usbdevice_fs.h>

// Boost Time
#include <boost/date_time/posix_time/posix_time.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

// Boost Threading
#include <boost/thread/thread.hpp>

// Boost INI Parser Includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// Simple Serial Interface
#include "lars_timeoutserial.h"

// Define the Speed of Light
#define CONST_c 299792458.0f

namespace lars
{

    // Setup TTY Structure
    struct tty
    {
        // Device Short-Name
        std::string name;

        // Device Address/Location
        std::string address;

        // Device Serial Number
        std::string serialNumber;

        // Device Baud Rate
        int baud;
    };

    // Setup Single Channel Structure
    struct channel_collection
    {
        // Timestamp
        std::string timestamp;

        // Store Frequency List
        std::vector<float> frequency;

        // Store Complex Channel Data
        std::vector<std::complex<float>> channel_A, channel_B;

        // Constructor
        channel_collection ( )
        {
            timestamp = "";
            frequency = { };
            channel_A = { };
            channel_B = { };
        }
    };

    // Setup Sweep Structure
    struct sweep
    {
        // Timestamp
        std::string timestamp;

        // Azimuth (in Degrees)
        int azimuth_current, azimuth_min, azimuth_max, azimuth_inc;

        // Inclinometers
        float inc_horn, inc_back, inc_front;

        // Temperatures
        float therm_box, therm_horn, therm_rf;

        // FPGA Values
        float fpga_sysclock;
        int fpga_freq_div, fpga_duty_div;
        int fpga_switches, fpga_pwm;

        // Network Analyzer Values
        float nwa_start_freq, nwa_stop_freq, nwa_span_freq, nwa_output_power, nwa_sweep_time;
        int nwa_points;
        struct channel_collection nwa_tx_off, nwa_tx_calibrate, nwa_tx_horizontal, nwa_tx_vertical;

        // Constructor
        sweep ( )
        {
            timestamp = "";
            azimuth_current = 0;
            azimuth_min = 0;
            azimuth_max = 0;
            azimuth_inc = 0;
            inc_horn = 0.0f;
            inc_back = 0.0f;
            inc_front = 0.0f;
            therm_box = 0.0f;
            therm_horn = 0.0f;
            therm_rf = 0.0f;
            fpga_sysclock = 0.0f;
            fpga_freq_div = 0;
            fpga_duty_div = 0;
            fpga_switches = 0;
            fpga_pwm = 0;
            nwa_start_freq = 0.0f;
            nwa_stop_freq = 0.0f;
            nwa_span_freq = 0.0f;
            nwa_output_power = 0.0f;
            nwa_sweep_time = 0.0f;
            nwa_points = 0;
        }
    };

    // Setup Serial Port Locations
    extern tty tty_A;
    extern tty tty_B;
    extern tty tty_C;
    extern tty tty_D;
    extern tty tty_E;
    extern tty tty_F;
    extern tty tty_G;
    extern tty tty_H;
    extern tty tty_I;
    extern tty tty_J;

    // Setup Network Analyzer (Attached via GPIB Controller)
    extern tty tty_NWA;

    // Setup Azimuth Parameters
    extern int azim_min_angle;
    extern int azim_max_angle;
    extern int azim_rest_angle;
    extern int azim_start_angle;
    extern int azim_tolerance;
    extern int azim_timeout;

    // Find a TTY device by name
    tty find_tty_device ( const std::string& device_name );

    // Parse TTY Configuration File
    bool load_device_address_configuration ( const std::string& filename );

    // Parse Azimuth Configuration File
    bool load_azimuth_configuration ( const std::string& filename );

    // Output a string timestamp
    const std::string stamp ( );
    const std::string stamp_date_only ( );

    // Reset the TTY Controller
    bool reset_usb_adapter ( tty ttyDevice );

    // Read output from the console after issuing a command
    std::string GetStdoutFromCommand ( const std::string& cmd );

    // Write sweep to ascii file
    bool write_sweep_to_ascii_file ( sweep sweep_data, std::string folder = "collections" );

}

#endif /* LARS_VARIABLES_H */