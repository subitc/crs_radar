/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: David
 *
 * Created on March 30, 2016, 12:27 PM
 */

#include <cstdlib>

// LARS Global Variables
#include "lars_variables.h"

// LARS Network Analyzer
#include "lars_networkanalyzer.h"

// LARS FPGA Control Board
#include "lars_fpga.h"

// LARS Azimuth Controller
#include "lars_azimuth.h"

// Test Function - FPGA Control Board
bool test_fpga ( )
{
    // Find the FPGA Board
    lars::tty tty_fpga = lars::find_tty_device( "FPGA" );

    // Check if the fpga initialized
    std::cout << "-- Testing \"is_initialized\"\n";
    lars::fpga fpga( tty_fpga );
    if ( not fpga.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }

    // Test "set_frequency"
    std::cout << "-- Testing \"set_frequency\"\n";
    for ( float desired_f = 1.0e6f; desired_f < 100.0e6f; desired_f += 1.0e6f )
    {
        // Returns actual value set due to rounding
        float actual_frequency = fpga.set_frequency( desired_f, 0.5f );

        std::cout << "   Set frequency to " << std::to_string( desired_f ) << ", actual = " << std::to_string( actual_frequency ) << "\n";

        // Sleep for 10 milliseconds
        boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
    }

    // Test "set_range"
    std::cout << "-- Testing \"set_range\"\n";
    for ( float desired_r = 1.0e3f; desired_r < 4.9e4f; desired_r += 1.0e3f )
    {
        // Returns actual value set due to rounding
        float actual_range = fpga.set_range( desired_r, 0.5f );

        std::cout << "   Set range to " << std::to_string( desired_r ) << ", actual = " << std::to_string( actual_range ) << "\n";

        // Sleep for 10 milliseconds
        boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
    }

    // Test "set_tx_direction = OFF"
    std::cout << "-- Testing \"set_tx_direction\" to TX_DIR_OFF\n";
    if ( not fpga.set_tx_direction( lars::fpga::tx_dir::TX_DIR_OFF ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set tx direction to OFF.\n";
        return false;
    }

    // Test "set_tx_direction = HORIZONTAL"
    std::cout << "-- Testing \"set_tx_direction\" to TX_DIR_HORIZONTAL\n";
    if ( not fpga.set_tx_direction( lars::fpga::tx_dir::TX_DIR_HORIZONTAL ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set tx direction to HORIZONTAL.\n";
        return false;
    }

    // Test "set_tx_direction = VERTICAL"
    std::cout << "-- Testing \"set_tx_direction\" to TX_DIR_VERTICAL\n";
    if ( not fpga.set_tx_direction( lars::fpga::tx_dir::TX_DIR_VERTICAL ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set tx direction to VERTICAL.\n";
        return false;
    }

    // Test "set_tx_direction = CALIBRATE"
    std::cout << "-- Testing \"set_tx_direction\" to TX_DIR_CALIBRATE\n";
    if ( not fpga.set_tx_direction( lars::fpga::tx_dir::TX_DIR_CALIBRATE ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set tx direction to CALIBRATE.\n";
        return false;
    }

    // Test "set_rxA_switch"
    std::cout << "-- Testing \"set_rxA_switch\"\n";
    if ( not fpga.set_rxA_switch( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_rxA_switch to 0.\n";
        return false;
    }
    if ( not fpga.set_rxA_switch( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_rxA_switch to 1.\n";
        return false;
    }

    // Test "set_rxB_switch"
    std::cout << "-- Testing \"set_rxB_switch\"\n";
    if ( not fpga.set_rxB_switch( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_rxB_switch to 0.\n";
        return false;
    }
    if ( not fpga.set_rxB_switch( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_rxB_switch to 1.\n";
        return false;
    }

    // Test "set_tx_switch"
    std::cout << "-- Testing \"set_tx_switch\"\n";
    if ( not fpga.set_tx_switch( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_tx_switch to 0.\n";
        return false;
    }
    if ( not fpga.set_tx_switch( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_tx_switch to 1.\n";
        return false;
    }

    // Test "set_pwm_enabled"
    std::cout << "-- Testing \"set_pwm_enabled\"\n";
    if ( not fpga.set_pwm_enabled( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_enabled to 0.\n";
        return false;
    }
    if ( not fpga.set_pwm_enabled( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_enabled to 1.\n";
        return false;
    }

    // Test "set_pwm_chA_enabled"
    std::cout << "-- Testing \"set_pwm_chA_enabled\"\n";
    if ( not fpga.set_pwm_chA_enabled( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_chA_enabled to 0.\n";
        return false;
    }
    if ( not fpga.set_pwm_chA_enabled( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_chA_enabled to 1.\n";
        return false;
    }

    // Test "set_pwm_chB_enabled"
    std::cout << "-- Testing \"set_pwm_chB_enabled\"\n";
    if ( not fpga.set_pwm_chB_enabled( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_chB_enabled to 0.\n";
        return false;
    }
    if ( not fpga.set_pwm_chB_enabled( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_chB_enabled to 1.\n";
        return false;
    }

    // Test "set_pwm_tx_enabled"
    std::cout << "-- Testing \"set_pwm_tx_enabled\"\n";
    if ( not fpga.set_pwm_tx_enabled( 0 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_tx_enabled to 0.\n";
        return false;
    }
    if ( not fpga.set_pwm_tx_enabled( 1 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set set_pwm_tx_enabled to 1.\n";
        return false;
    }

    // Return success
    return true;
}

// Test Function - Network Analyzer
bool test_network_analyzer ( )
{
    // Find the GPIB Adapter for the NWA
    lars::tty tty_nwa = lars::find_tty_device( "GPIB" );

    // Check if the device was initialized
    std::cout << "-- Testing \"is_initialized\"\n";
    lars::network_analyzer hp8753d( tty_nwa );
    if ( not hp8753d.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }

    // Setup a collection
    hp8753d.setup_collection( );

    // Return success
    return true;
}

// Test Function - Azimuth Controller
bool test_azimuth_controller ( )
{
    // Find the serial adapter for the azimuth controller
    lars::tty tty_azim = lars::find_tty_device( "AZIM" );

    // Check if the device was initialized
    std::cout << "-- Testing \"is_initialized\"\n";
    lars::azimuth azimuth_controller( tty_azim );
    if ( not azimuth_controller.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }

    // Test setting azimuth "value"
    std::cout << "-- Testing setting azimuth \"value\"\n";
    std::cout << "    Setting azimuth controller to minimum angle." << std::endl;
    if ( not azimuth_controller.value( lars::azim_min_angle ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set azimuth to minimum angle.\n";
        return false;
    }
    std::cout << "    Setting azimuth controller to maximum angle." << std::endl;
    if ( not azimuth_controller.value( lars::azim_max_angle ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set azimuth to maximum angle.\n";
        return false;
    }
    std::cout << "    Setting azimuth controller to starting angle." << std::endl;
    if ( not azimuth_controller.value( lars::azim_start_angle ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set azimuth to starting angle.\n";
        return false;
    }

    // Test "manual_rotate" left for 10 seconds
    std::cout << "-- Testing \"manual_rotate\"\n";
    std::cout << "    Rotating device ROTATE_CLOCKWISE for 5 seconds.\n";
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_CLOCKWISE, 5 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate left for 5 seconds.\n";
        return false;
    }
    std::cout << "    Rotating device ROTATE_COUNTER_CLOCKWISE for 5 seconds.\n";
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_COUNTER_CLOCKWISE, 5 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate right for 5 seconds.\n";
        return false;
    }
    std::cout << "    Rotating device ROTATE_CLOCKWISE for 10 seconds.\n";
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_CLOCKWISE, 10 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate left for 10 seconds.\n";
        return false;
    }
    std::cout << "    Rotating device ROTATE_COUNTER_CLOCKWISE for 10 seconds.\n";
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_COUNTER_CLOCKWISE, 10 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate right for 10 seconds.\n";
        return false;
    }

    // Test "set_rotation_speed" to SPEED_STAGNANT and move left
    std::cout << "-- Testing \"set_rotation_speed\" to SPEED_STAGNANT\n";
    if ( not azimuth_controller.set_rotation_speed( lars::azimuth::speed_mode::SPEED_STAGNANT ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set rotation speed to SPEED_STAGNANT.\n";
        return false;
    }


    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_CLOCKWISE, 5 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate left for 5 seconds.\n";
        return false;
    }

    // Test "set_rotation_speed" to SPEED_SLOW and move left
    std::cout << "-- Testing \"set_rotation_speed\" to SPEED_SLOW\n";
    if ( not azimuth_controller.set_rotation_speed( lars::azimuth::speed_mode::SPEED_SLOW ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set rotation speed to SPEED_SLOW.\n";
        return false;
    }
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_CLOCKWISE, 5 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate left for 5 seconds.\n";
        return false;
    }

    // Test "set_rotation_speed" to SPEED_MEDIUM and move left
    std::cout << "-- Testing \"set_rotation_speed\" to SPEED_MEDIUM\n";
    if ( not azimuth_controller.set_rotation_speed( lars::azimuth::speed_mode::SPEED_MEDIUM ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set rotation speed to SPEED_MEDIUM.\n";
        return false;
    }
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_CLOCKWISE, 5 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate left for 5 seconds.\n";
        return false;
    }

    // Test "set_rotation_speed" to STAGNANT and move left
    std::cout << "-- Testing \"set_rotation_speed\" to SPEED_FAST\n";
    if ( not azimuth_controller.set_rotation_speed( lars::azimuth::speed_mode::SPEED_FAST ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set rotation speed to SPEED_FAST.\n";
        return false;
    }
    if ( not azimuth_controller.manual_rotate( lars::azimuth::rotate_mode::ROTATE_CLOCKWISE, 5 ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to run manual_rotate left for 5 seconds.\n";
        return false;
    }

    // Return to starting position
    std::cout << "-- Returning azimuth controller to resting angle." << std::endl;
    if ( not azimuth_controller.value( lars::azim_rest_angle ) )
    {
        std::cout << "(Line " << __LINE__ << ") Failed to set azimuth to resting angle.\n";
        return false;
    }

    // Return success
    return true;
}

// Test RF Front-End Switches
bool test_rf_switches ( )
{
    // -- Setup the FPGA
    std::cout << "-- Setting up the FPGA.\n";

    // Find the FPGA Board
    lars::tty tty_fpga = lars::find_tty_device( "FPGA" );

    // Check if the fpga initialized
    lars::fpga fpga( tty_fpga );
    if ( not fpga.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the FPGA serial interface.\n";
        return false;
    }

    // Set Hardware Gating for 10 meters
    fpga.set_range( 10.0f, 0.5 );

    // -- Setup the Network Analyzer
    std::cout << "-- Setting up the network analyzer.\n";

    // Find the GPIB Adapter for the NWA
    lars::tty tty_nwa = lars::find_tty_device( "GPIB" );

    // Check if the device was initialized
    lars::network_analyzer hp8753d( tty_nwa );
    if ( not hp8753d.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }

    // Setup a collection
    std::cout << "-- Setting network analyzer to active operation mode.\n";
    hp8753d.setup_collection( );

    // Disable Hardware Gating
    std::cout << "-- Setting FPGA to active operation mode.\n";
    fpga.set_pwm_chA_enabled( 0 );
    fpga.set_pwm_chB_enabled( 0 );
    fpga.set_pwm_tx_enabled( 0 );

    // Cycle through all switches
    for ( int switch_mode = 0; switch_mode < 8; switch_mode++ )
    {
        // Setup a test sweep
        lars::sweep test_sweep;
        test_sweep.timestamp.assign( lars::stamp( ) );
        test_sweep.fpga_switches = switch_mode;

        // Adjust FPGA Parameters
        std::cout << "      Setting FPGA switch mode to " << switch_mode << ".\n";
        bool state_rxA = ( switch_mode >> 0 ) & 1; // Test Bit 0
        bool state_rxB = ( switch_mode >> 1 ) & 1; // Test Bit 1
        bool state_tx = ( switch_mode >> 2 ) & 1; // Test Bit 2
        fpga.set_rxB_switch( state_rxB );
        fpga.set_tx_switch( state_tx );
        fpga.set_rxA_switch( state_rxA );

        // Run a sweep
        std::cout << "      Sweeping NWA.\n";
        hp8753d.nwa_run_sweep( test_sweep, fpga );

        // Store sweep data to file
        std::cout << "      Storing sweep to file.\n";
        lars::write_sweep_to_ascii_file( test_sweep, "rf_switch_tests" );
    }

    // Return success
    return true;
}


// Test the FPGA Hardware Gating Feature
bool test_hardware_gating ( bool hardware_gating )
{
    // -- Setup the FPGA
    std::cout << "-- Setting up the FPGA.\n";

    // Find the FPGA Board
    lars::tty tty_fpga = lars::find_tty_device( "FPGA" );

    // Check if the fpga initialized
    lars::fpga fpga( tty_fpga );
    if ( not fpga.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the FPGA serial interface.\n";
        return false;
    }

    // Setup Runtime Parameters
    std::cout << "-- Setting FPGA to active operation mode.\n";
    if ( hardware_gating )
    {
        // Set Hardware Gating for 10 meters
        fpga.set_range( 10.0f, 0.5 );
    }
    else
    {
        fpga.set_rxB_switch( false );
        fpga.set_tx_switch( false );
        fpga.set_rxA_switch( false );
    }
    fpga.set_pwm_chA_enabled( hardware_gating );
    fpga.set_pwm_chB_enabled( hardware_gating );
    fpga.set_pwm_tx_enabled( hardware_gating );

    // -- Setup the Network Analyzer
    std::cout << "-- Setting up the network analyzer.\n";

    // Find the GPIB Adapter for the NWA
    lars::tty tty_nwa = lars::find_tty_device( "GPIB" );

    // Check if the device was initialized
    lars::network_analyzer hp8753d( tty_nwa );
    if ( not hp8753d.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }

    // Setup a collection
    std::cout << "-- Setting network analyzer to active operation mode.\n";
    hp8753d.setup_collection( );

    // Setup a test sweep
    lars::sweep test_sweep;
    test_sweep.timestamp.assign( lars::stamp( ) );

    // Run a sweep
    std::cout << "      Sweeping NWA.\n";
    hp8753d.nwa_run_sweep( test_sweep, fpga );

    // Store sweep data to file
    std::cout << "      Storing sweep to file.\n";
    lars::write_sweep_to_ascii_file( test_sweep, "hardware_gating_tests" );

    // Return success
    return true;
}


// Test the FPGA Hardware Gating Feature
bool test_run_full_scan ( bool hardware_gating, int azim_start, int azim_stop, int azim_inc )
{
    // -- Setup the FPGA
    std::cout << "-- Setting up the FPGA.\n";

    // Find the FPGA Board
    lars::tty tty_fpga = lars::find_tty_device( "FPGA" );

    // Check if the fpga initialized
    lars::fpga fpga( tty_fpga );
    if ( not fpga.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the FPGA serial interface.\n";
        return false;
    }

    // Setup Runtime Parameters
    std::cout << "-- Setting FPGA to active operation mode.\n";
    if ( hardware_gating )
    {
        // Set Hardware Gating for 10 meters
        fpga.set_range( 10.0f, 0.5 );
    }
    else
    {
        fpga.set_rxB_switch( false );
        fpga.set_tx_switch( false );
        fpga.set_rxA_switch( false );
    }
    fpga.set_pwm_chA_enabled( hardware_gating );
    fpga.set_pwm_chB_enabled( hardware_gating );
    fpga.set_pwm_tx_enabled( hardware_gating );


    // -- Setup the Network Analyzer
    std::cout << "-- Setting up the network analyzer.\n";

    // Find the GPIB Adapter for the NWA
    lars::tty tty_nwa = lars::find_tty_device( "GPIB" );

    // Check if the device was initialized
    lars::network_analyzer hp8753d( tty_nwa );
    if ( not hp8753d.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }

    // Setup a collection
    std::cout << "-- Setting network analyzer to active operation mode.\n";
    hp8753d.setup_collection( );


    // -- Setup the Azimuth Controller
    std::cout << "-- Setting up the Azimuth Controller.\n";

    // Find the serial adapter for the azimuth controller
    lars::tty tty_azim = lars::find_tty_device( "AZIM" );

    // Check if the device was initialized
    std::cout << "-- Testing \"is_initialized\"\n";
    lars::azimuth azimuth_controller( tty_azim );
    if ( not azimuth_controller.is_initialized( ) )
    {
        std::cout << "(Line " << __LINE__ << ") Could not initialize the serial interface.\n";
        return false;
    }


    // -- SCAN --

    // Iterate through azimuth
    for ( int azim_current = azim_start; azim_current < azim_stop; azim_current += azim_inc )
    {
        // Setup a test sweep
        lars::sweep test_sweep;
        test_sweep.timestamp.assign( lars::stamp( ) );

        // Send azimuth controller to azim_current
        std::cout << "    Setting azimuth to: " << azim_current << " degrees.\n";
        test_sweep.azimuth_current = azim_current;
        test_sweep.azimuth_min = azim_start;
        test_sweep.azimuth_max = azim_stop;
        if ( not azimuth_controller.value( azim_current ) )
        {
            std::cout << "(Line " << __LINE__ << ") Failed to set azimuth to: " << azim_current << " degrees.\n";
            return false;
        }

        // Run a sweep
        std::cout << "      Sweeping NWA.\n";
        hp8753d.nwa_run_sweep( test_sweep, fpga );

        // Store sweep data to file
        std::cout << "      Storing sweep to file.\n";
        std::string store_foldername = "collections/" + lars::stamp_date_only( );
        lars::write_sweep_to_ascii_file( test_sweep, store_foldername );
    }

    // Return success
    return true;
}



// Main Routine
int main ( int argc, char** argv )
{
    
    // Test Loading Device Address Configuration File
    std::cout << "Starting Device Address Configuration File Test...\n";
    if ( lars::load_device_address_configuration( "config/device_addresses.ini" ) )
        std::cout << "Device Address Configuration: PASSED" << std::endl;
    else
        std::cout << "Device Address Configuration: FAILED" << std::endl;


    // Test Loading Azimuth Configuration File
    if ( lars::load_azimuth_configuration( "config/azimuth_parameters.ini" ) )
        std::cout << "Azimuth Controller Configuration: PASSED\n" << std::endl;
    else
        std::cout << "Azimuth Controller Configuration: FAILED\n" << std::endl;

    /*
    // Test Azimuth Controller
    std::cout << "Starting Azimuth Controller Tests...\n";
    if (test_azimuth_controller ())
        std::cout << "Azimuth Controller : PASSED\n" << std::endl;
    else
        std::cout << "Azimuth Controller : FAILED\n" << std::endl;
    
    //Test FPGA Control Board
    std::cout << "Starting FPGA Control Board Tests...\n";
    if ( test_fpga( ) )
        std::cout << "FPGA Control Board: PASSED\n" << std::endl;
    else
        std::cout << "FPGA Control Board: FAILED\n" << std::endl;
	
    /*
    // Test GPIB Interface Network Analyzer
    std::cout << "Starting HP8753D Network Analyzer Tests...\n";
    if ( test_network_analyzer( ) )
        std::cout << "HP8753D Network Analyzer: PASSED\n" << std::endl;
    else
        std::cout << "HP8753D Network Analyzer: FAILED\n" << std::endl;
    */
    /*
    // Test RF Front-End Switches
    
    std::cout << "Starting RF Front-End Switching Tests...\n";
    if ( test_rf_switches( ) )
        std::cout << "RF Front-End Switching: PASSED\n" << std::endl;
    else
        std::cout << "RF Front-End Switching: FAILED\n" << std::endl;
     

    
    // Test Hardware Gating
   std::cout << "Starting Hardware Gating Tests...\n";
    if ( test_hardware_gating( false ) )
        std::cout << "Hardware Gating (false) : PASSED\n" << std::endl;
    else
        std::cout << "Hardware Gating (false) : FAILED\n" << std::endl;
    if ( test_hardware_gating( true ) )
        std::cout << "Hardware Gating (true) : PASSED\n" << std::endl;
    else
        std::cout << "Hardware Gating (true) : FAILED\n" << std::endl;
    
    */
    // Test Azimuth Sweep
    
    std::cout << "Starting Partial Sweep Collection Tests...\n";
    if ( test_run_full_scan(false,250,265,5 ))
        std::cout << "Partial Sweep Collection: PASSED\n" << std::endl;
    else
        std::cout << "Partial Sweep Collection: FAILED\n" << std::endl;
     
    // Complete
    return 0;
}



