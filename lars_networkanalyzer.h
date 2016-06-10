/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_networkanalyzer.h
 * Author: David
 *
 * Created on March 30, 2016, 1:44 PM
 */

#ifndef LARS_NETWORKANALYZER_H
#define LARS_NETWORKANALYZER_H

// LARS Variables
#include "lars_variables.h"

// LARS FPGA
#include "lars_fpga.h"

namespace lars
{
    class network_analyzer
    {
    public:

        // Constructor
        network_analyzer ( tty ttyDevice );

        // Destructor
        virtual ~network_analyzer ( );

        // Update the assigned TTY
        void update_tty ( tty ttyDevice );

        // Check if the device was initialized
        bool is_initialized ( );

        // Setup a collection
        bool setup_collection (
                                float frequency_start = 1100.0e6,
                                float frequency_stop = 1400.0e6,
                                int points = 201,
                                float power = 0.0e0
                                );

        // NWA Specific Commands
        bool nwa_print_version ( );
        bool nwa_rest_to_presets ( );
        bool nwa_set_debugging ( bool debug_enabled );
        std::string nwa_date ( );
        std::string nwa_time ( );
        bool nwa_date ( int year, int month, int day );
        bool nwa_time ( int hour, int minute, int second );
        bool nwa_date_to_system ( );
        bool nwa_time_to_system ( );
        bool nwa_clear_status_registers ( );
        bool nwa_set_frequency_range ( float start_frequency, float end_frequency );
        bool nwa_set_collection_points ( int desired_points );
        bool nwa_set_output_power_dbm ( float output_power_dbm );
        bool nwa_toggle_power_trip ( );
        bool nwa_set_channel_coupling ( bool couple_channels );
        bool nwa_set_dual_display ( bool dual_display );
        bool nwa_blank_screen ( bool blank_screen );
        bool nwa_set_current_channel ( int channel );
        bool nwa_set_mode_polar ( );
        bool nwa_autoscale_display ( );
        bool nwa_set_averaging ( bool averaging_enabled );
        int nwa_run_sweep ( lars::sweep &nwa_sweep, lars::fpga &fpga_device );

    private:

        // Private Methods
        bool gpib_initialize ( );

        // GPIB Specific Commands
        bool gpib_print_version ( );
        bool gpib_reset_controller ( );
        bool gpib_set_save_configuration ( bool gpib_config );
        bool gpib_set_mode ( int gpib_mode );
        bool gpib_set_timeout ( int gpib_milliseconds );
        bool gpib_set_auto ( bool gpib_auto );
        bool gpib_set_address ( int gpib_address );
        bool gpib_set_eoi ( bool gpib_eoi );
        bool gpib_set_eos ( int gpib_eos );

        // Private Serial Interface
        TimeoutSerial serial;
        std::string dev_name; // TTY device name
        std::string dev_address; // TTY device address
        std::string dev_serialnumber; // TTY device serial number
        int dev_baud; // TTY device baud

        // Private Variables
        bool initialized; // Was the device able to initialize?
    };

}

#endif /* LARS_NETWORKANALYZER_H */

