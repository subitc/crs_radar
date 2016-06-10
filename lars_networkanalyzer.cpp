/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   network_analyzer.cpp
 * Author: David
 *
 * Created on March 30, 2016, 1:44 PM
 */

// LARS Network Analyzer
#include "lars_networkanalyzer.h"

// LARS Global Variables
#include "lars_variables.h"
#include "lars_fpga.h"

// LARS Network Analyzer Constructor
lars::network_analyzer::network_analyzer ( tty ttyDevice )
{
    // Declare device as uninitialized
    initialized = false;

    // Store the device's TTY
    update_tty( ttyDevice );

    // Set the timeout period
    serial.setTimeout( boost::posix_time::seconds( 30 ) );

    // Reset the Serial Adapter
    if ( reset_usb_adapter( ttyDevice ) )
    {
        // Test the serial port
        serial.open( dev_address, dev_baud );
        if ( serial.isOpen( ) )
        {
            // Port opened - close and report it's functional
            serial.close( );
            std::cout << stamp( ) << " [" << dev_name << "] Serial port is functional." << std::endl;
        }
        else
        {
            // Port wasn't open - report it's not responding to our command
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "port did not respond to 'open' command." << __func__ << "." << std::endl;
        }

        // Initialize Controller
        if ( network_analyzer::gpib_initialize( ) == false )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "could initialize the GPIB device to default settings." << __func__ << "." << std::endl;
        }

        // Finish Initialization
        initialized = true;
    }
}

// LARS Network Analyzer Destructor
lars::network_analyzer::~network_analyzer ( )
{
    // Print Goodbye
    std::cout << stamp( ) << " [" << dev_name << "] Closing port." << std::endl;

    // Close the serial port
    if ( serial.isOpen( ) ) serial.close( );
}

// Set the assigned TTY device
void lars::network_analyzer::update_tty ( tty ttyDevice )
{
    // Store the device's name
    dev_name.assign( ttyDevice.name );

    // Store the device's address
    dev_address.assign( ttyDevice.address );

    // Store the device's serial number
    dev_serialnumber.assign( ttyDevice.serialNumber );

    // Store the device's baud rate
    dev_baud = ttyDevice.baud;
}

// Check if the device was initialized
bool lars::network_analyzer::is_initialized ( )
{
    return initialized;
}

// Initialize the GPIB Controller
bool lars::network_analyzer::gpib_initialize ( )
{
    // Setup Prologix USB to GPIB Adapter
    try
    {
        // Reset the controller
        gpib_reset_controller( );

        // Print the firmware version of the Prologix Controller
        gpib_print_version( );

        // Setup the controller's configuration
        std::cout << stamp( ) << " [" << dev_name << "] Starting configuration of the GPIB controller." << std::endl;

        // Tell the controller to save configuration on power-loss
        gpib_set_save_configuration( true );

        // Set the read timeout to 3 seconds
        gpib_set_timeout( 3000 );

        // Set to DEVICE mode
        gpib_set_mode( 1 );

        // Set auto to true (read after write mode)
        gpib_set_auto( true );

        // Set the address of the network analyzer
        gpib_set_address( 16 );

        // Set the end character mode
        gpib_set_eoi( true );

        // Set the end character used
        gpib_set_eos( 1 );

        // GPIB Controller Finished Initialization
        std::cout << stamp( ) << " [" << dev_name << "] Completed configuration of the GPIB controller." << std::endl;

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Setup a collection
/* frequency_start     = Starting Frequency
 * frequency_stop      = Ending Frequency
 * points              = Number of collection points
 * power               = Power Output
 */
bool lars::network_analyzer::setup_collection (
                                                float frequency_start,
                                                float frequency_stop,
                                                int points,
                                                float power
                                                )
{
    // Setup Prologix USB to GPIB Adapter
    try
    {

        // Setup the controller's configuration
        std::cout << stamp( ) << " [" << dev_name << "] Starting to setup a collection." << std::endl;

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Requesting the NWA's product identification." << std::endl;

        // Get the NWA's Identification
        nwa_print_version( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting NWA to preset values." << std::endl;

        // Start with the NWA Presets
        nwa_rest_to_presets( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Disabling the NWA debugger." << std::endl;

        // Disable the NWA Debugger
        nwa_set_debugging( false );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting the network analyzer's date to system date." << std::endl;

        // Set the network analyzer's date to the current system date
        nwa_date_to_system( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting the network analyzer's time to system time." << std::endl;

        // Set the network analyzer's time to the current system time
        nwa_time_to_system( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Checking date on the network analyzer." << std::endl;

        // Print out the date on the network analyzer
        std::cout << stamp( ) << " [" << dev_name << "] NWA Date: " << nwa_date( ) << std::endl;

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Checking time on the network analyzer." << std::endl;

        // Print out the time on the network analyzer
        std::cout << stamp( ) << " [" << dev_name << "] NWA Time: " << nwa_time( ) << std::endl;

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Clearing all NWA status registers." << std::endl;

        // Clear all NWA status registers
        nwa_clear_status_registers( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting the NWA frequency range [1.1e9, 1.4e9]." << std::endl;

        // Set the NWA frequency range [1.1e9, 1.4e9]
        nwa_set_frequency_range( 1.1e9f, 1.4e9f );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting the NWA collection points to 201." << std::endl;

        // Set the NWA collection points to 201
        nwa_set_collection_points( 201 );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting the NWA output power to 0.00dBm." << std::endl;

        // Setting the NWA output power to 0.00dBm
        nwa_set_output_power_dbm( 0.0f );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting the NWA averaging mode to off." << std::endl;

        // Setting the NWA averaging mode to off
        nwa_set_averaging( false );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Toggle the NWA's power trip." << std::endl;

        // Toggle the NWA's power trip
        nwa_toggle_power_trip( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting NWA channel coupling." << std::endl;

        // Set NWA channel coupling
        nwa_set_channel_coupling( true );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting NWA to dual display mode." << std::endl;

        // Setting NWA to dual display mode
        nwa_set_dual_display( true );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting NWA to blank screen mode." << std::endl;

        // Setting NWA to blank screen mode
        nwa_blank_screen( false );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting NWA Channel 1 to polar, form 4." << std::endl;

        // Setting NWA Channel 1 to polar, form 4
        nwa_set_current_channel( 1 );
        nwa_set_mode_polar( );
        nwa_autoscale_display( );

        // Print a status message
        std::cout << stamp( ) << " [" << dev_name << "] Setting NWA Channel 2 to polar, form 4." << std::endl;

        // Setting NWA Channel 2 to polar, form 4
        nwa_set_current_channel( 2 );
        nwa_set_mode_polar( );
        nwa_autoscale_display( );

        // GPIB Controller Finished Initialization
        std::cout << stamp( ) << " [" << dev_name << "] Finished setting up a collection." << std::endl;

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }

};


// Obtain the date on the network analyzer
// RETURNS STRING // Ex: 31 Mar 2016
std::string lars::network_analyzer::nwa_date ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Request the device's version information

        serial.writeString( "\nREADDATE;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return nwa_string;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Obtain the time on the network analyzer
// RETURNS STRING // Ex: 11:21:13
std::string lars::network_analyzer::nwa_time ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Request the device's version information
        serial.writeString( "\nREADTIME;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return nwa_string;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set the date on the network analyzer
// SETDATE DD MMM YYYY;
bool lars::network_analyzer::nwa_date ( int year, int month, int day )
{
    try
    {
        // Check the input year
        if ( ( year < 0 ) or (year > 9999 ) )
            throw timeout_exception( "Input year out of range [0, 9999]." );

        // Check the input month
        if ( ( month < 1 ) or (month > 12 ) )
            throw timeout_exception( "Input month out of range [1, 12]." );

        // Check the input day
        if ( ( day < 1 ) or (day > 31 ) )
            throw timeout_exception( "Input day out of range [1, 31]." );

        // Convert month to string
        std::string desired_month;
        switch ( month )
        {
        case 1:
            desired_month.assign( "Jan" );
            break;
        case 2:
            desired_month.assign( "Feb" );
            break;
        case 3:
            desired_month.assign( "Mar" );
            break;
        case 4:
            desired_month.assign( "Apr" );
            break;
        case 5:
            desired_month.assign( "May" );
            break;
        case 6:
            desired_month.assign( "Jun" );
            break;
        case 7:
            desired_month.assign( "Jul" );
            break;
        case 8:
            desired_month.assign( "Aug" );
            break;
        case 9:
            desired_month.assign( "Sep" );
            break;
        case 10:
            desired_month.assign( "Oct" );
            break;
        case 11:
            desired_month.assign( "Nov" );
            break;
        case 12:
            desired_month.assign( "Dec" );
            break;
        default:
            // Should never reach this due to check above
            desired_month.assign( "Dec" );
            break;
        }

        // Build the command
        std::stringstream sstream;
        sstream << "\nSETDATE "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( day )
                << " "
                << desired_month
                << " "
                << std::setfill( '0' )
                << std::setw( 4 )
                << std::to_string( year )
                << ";\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the string command
        serial.writeString( sstream.str( ) );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set the time on the network analyzer
// SETTIME HH:MM:SS;
bool lars::network_analyzer::nwa_time ( int hour, int minute, int second )
{
    try
    {
        // Check the input year
        if ( ( hour < 0 ) or (hour > 23 ) )
            throw timeout_exception( "Input hour out of range [0, 23]." );

        // Check the input month
        if ( ( minute < 0 ) or (minute > 59 ) )
            throw timeout_exception( "Input minute out of range [0, 59]." );

        // Check the input day
        if ( ( second < 0 ) or (second > 59 ) )
            throw timeout_exception( "Input second out of range [0, 59]." );

        // Build the command
        std::stringstream sstream;
        sstream << "\nSETTIME "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( hour )
                << ":"
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( minute )
                << ":"
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( second )
                << ";\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the string command
        serial.writeString( sstream.str( ) );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set the date on the network analyzer
// SETDATE DD MMM YYYY;
bool lars::network_analyzer::nwa_date_to_system ( )
{
    try
    {
        // Retrieve the current system date/time
        // Ex: "2016-05-09T18:30:21+0000"
        std::string current_system_datetime = GetStdoutFromCommand( "date -Iseconds" );

        // Parse the hour, minute, and second
        int year = std::stoi( current_system_datetime.substr( 0, 4 ) );
        int month = std::stoi( current_system_datetime.substr( 5, 2 ) );
        int day = std::stoi( current_system_datetime.substr( 8, 2 ) );

        // Check the input year
        if ( ( year < 0 ) or (year > 9999 ) )
            throw timeout_exception( "Input year out of range [0, 9999]." );

        // Check the input month
        if ( ( month < 1 ) or (month > 12 ) )
            throw timeout_exception( "Input month out of range [1, 12]." );

        // Check the input day
        if ( ( day < 1 ) or (day > 31 ) )
            throw timeout_exception( "Input day out of range [1, 31]." );

        // Convert month to string
        std::string desired_month;
        switch ( month )
        {
        case 1:
            desired_month.assign( "Jan" );
            break;
        case 2:
            desired_month.assign( "Feb" );
            break;
        case 3:
            desired_month.assign( "Mar" );
            break;
        case 4:
            desired_month.assign( "Apr" );
            break;
        case 5:
            desired_month.assign( "May" );
            break;
        case 6:
            desired_month.assign( "Jun" );
            break;
        case 7:
            desired_month.assign( "Jul" );
            break;
        case 8:
            desired_month.assign( "Aug" );
            break;
        case 9:
            desired_month.assign( "Sep" );
            break;
        case 10:
            desired_month.assign( "Oct" );
            break;
        case 11:
            desired_month.assign( "Nov" );
            break;
        case 12:
            desired_month.assign( "Dec" );
            break;
        default:
            // Should never reach this due to check above
            desired_month.assign( "Dec" );
            break;
        }

        // Build the command
        std::stringstream sstream;
        sstream << "\nSETDATE "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( day )
                << " "
                << desired_month
                << " "
                << std::setfill( '0' )
                << std::setw( 4 )
                << std::to_string( year )
                << ";\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the string command
        serial.writeString( sstream.str( ) );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set the time on the network analyzer to the current system time
// SETTIME HH:MM:SS;
bool lars::network_analyzer::nwa_time_to_system ( )
{
    try
    {
        // Retrieve the current system date/time
        // Ex: "2016-05-09T18:30:21+0000"
        std::string current_system_datetime = GetStdoutFromCommand( "date -Iseconds" );

        // Parse the hour, minute, and second
        int hour = std::stoi( current_system_datetime.substr( 11, 2 ) );
        int minute = std::stoi( current_system_datetime.substr( 14, 2 ) );
        int second = std::stoi( current_system_datetime.substr( 17, 2 ) );

        // Check the input year
        if ( ( hour < 0 ) or (hour > 23 ) )
            throw timeout_exception( "Input hour out of range [0, 23]." );

        // Check the input month
        if ( ( minute < 0 ) or (minute > 59 ) )
            throw timeout_exception( "Input minute out of range [0, 59]." );

        // Check the input day
        if ( ( second < 0 ) or (second > 59 ) )
            throw timeout_exception( "Input second out of range [0, 59]." );

        // Build the command
        std::stringstream sstream;
        sstream << "\nSETTIME "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( hour )
                << ":"
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( minute )
                << ":"
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::to_string( second )
                << ";\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the string command
        serial.writeString( sstream.str( ) );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Print the identifier and version of the network analyzer
bool lars::network_analyzer::nwa_print_version ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Request the device's version information
        serial.writeString( "\nIDN?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Print the response
        std::cout << stamp( ) << " [" << dev_name << "] Product ID: " << nwa_string << std::endl;

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Reset the network analyzer to preset values
bool lars::network_analyzer::nwa_rest_to_presets ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // -- Reset to presets

        // Send command
        serial.writeString( "\nOPC?; PRES;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != 1 )
            throw timeout_exception( "Setting NWA to preset values failed." );

        // -- Disable Transform Mode
        /*
        // Send command
        serial.writeString("\nOPC?; TIMDTRAN OFF;\n");

        // Capture result string
        nwa_string.assign(serial.readStringUntil("\n"));

        // Verify we've completed presets
        if (std::stoi(nwa_string) != 1)
            throw timeout_exception("Setting NWA to disable transform mode failed.");

        // -- Disable corrections and set to linear frequency sweep

        // Send command (no response expected)
        serial.writeString("\nCORR OFF; LINFREQ;\n");
         */

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Reset the network analyzer to preset values
bool lars::network_analyzer::nwa_set_debugging ( bool debug_enabled )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Decide if debugging is enabled or not
        if ( debug_enabled )
            // Request the device's version information
            serial.writeString( "\nDEBUON;\n" );
        else
            // Request the device's version information
            serial.writeString( "\nDEBUOFF;\n" );

        // Verify Integrity

        // Request the current debug mode from the NWA
        serial.writeString( "\nDEBU?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != debug_enabled )
            throw timeout_exception( "Setting the NWA debug mode failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Clear all status registers
// CLES;
bool lars::network_analyzer::nwa_clear_status_registers ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the command (no result expected)
        serial.writeString( "\nCLES;\n" );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set collection frequency range
// STOP %f MHz; STAR %f MHz;
bool lars::network_analyzer::nwa_set_frequency_range ( float start_frequency, float end_frequency )
{
    try
    {
        // Verify the integrity of the input starting frequency
        if ( ( start_frequency < 30.0e3f ) or (start_frequency > 3.0e9f ) )
            throw timeout_exception( "Starting frequency is out of range [30.0e3, 3.0e9]." );

        // Verify the integrity of the input ending frequency
        if ( ( end_frequency < 30.0e3f ) or (end_frequency > 3.0e9f ) )
            throw timeout_exception( "Ending frequency is out of range [30.0e3, 3.0e9]." );

        // Verify the starting frequency is smaller than the ending frequency
        if ( end_frequency < start_frequency )
            throw timeout_exception( "Ending frequency must be larger than starting frequency." );

        // Reduce input value (starting frequency) to 3 digit precision
        std::stringstream start_stream;
        start_stream << std::setprecision( 3 )
                << std::scientific
                << start_frequency;
        start_frequency = std::stof( start_stream.str( ) );

        // Reduce input value (ending frequency) to 3 digit precision
        std::stringstream end_stream;
        end_stream << std::setprecision( 3 )
                << std::scientific
                << end_frequency;
        end_frequency = std::stof( end_stream.str( ) );

        // Build the command
        std::stringstream sstream;
        sstream << "\nSTAR "
                << std::setprecision( 3 )
                << std::scientific
                << start_frequency
                << " Hz; STOP "
                << std::setprecision( 3 )
                << std::scientific
                << end_frequency
                << " Hz;\n";

        // Build the string
        std::string nwa_string = sstream.str( );

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send command
        serial.writeString( sstream.str( ) );

        // Verify Integrity

        // Request the current setting from the NWA
        serial.writeString( "\nSTOP?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Verify we've completed presets ( 10% threshold )
        float actual_end_frequency = std::stof( nwa_string );
        if ( actual_end_frequency != end_frequency )
            throw timeout_exception( "Setting the NWA ending frequency failed." );

        // Request the current setting from the NWA
        serial.writeString( "\nSTAR?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Verify we've completed presets ( 10% threshold )
        float actual_start_frequency = std::stof( nwa_string );
        if ( actual_start_frequency != start_frequency )
            throw timeout_exception( "Setting the NWA starting frequency failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set number of collection points
// POIN %d;
bool lars::network_analyzer::nwa_set_collection_points ( int desired_points )
{
    try
    {
        // Verify the integrity of the input
        if ( ( desired_points < 3 ) or (desired_points > 1024 ) )
            throw timeout_exception( "Input points is out of range [3, 1024]." );

        // Build the command
        std::stringstream sstream;
        sstream << "\nPOIN "
                << std::to_string( desired_points )
                << ";\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send command
        serial.writeString( sstream.str( ) );

        // Verify Integrity

        // Request the current setting from the NWA
        serial.writeString( "\nPOIN?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        int actual_points = ( int ) std::stof( nwa_string );
        if ( actual_points != desired_points )
            throw timeout_exception( "Setting the NWA points failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set collection output power (in dBm, 0.000f)
// POWE %f;
bool lars::network_analyzer::nwa_set_output_power_dbm ( float output_power_dbm )
{
    try
    {
        // Verify the integrity of the input
        if ( ( output_power_dbm < -10.0f ) or (output_power_dbm > 10.0f ) )
            throw timeout_exception( "Output power is out of range [-10.0, 10.0]." );

        // Reduce input value to 3 digit precision
        std::stringstream prec_reduction_stream;
        prec_reduction_stream << std::setprecision( 3 )
                << std::scientific
                << output_power_dbm;
        output_power_dbm = std::stof( prec_reduction_stream.str( ) );

        // Build the command
        std::stringstream sstream;
        sstream << "\nPOWE "
                << std::setprecision( 3 )
                << std::scientific
                << output_power_dbm
                << ";\n";

        // Build the string
        std::string nwa_string = sstream.str( );

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send command
        serial.writeString( sstream.str( ) );

        // Verify Integrity

        // Request the current setting from the NWA
        // Ex: "0.000000000000000E+00"
        // Ex: "1.000000000000000E+01"
        serial.writeString( "\nPOWE?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Verify we've completed presets ( 10% threshold )
        float actual_output_power_dbm = std::stof( nwa_string );
        if ( actual_output_power_dbm != output_power_dbm )
            throw timeout_exception( "Setting the NWA output power failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Toggle NWA power trip
// POWT OFF; POWT ON;
bool lars::network_analyzer::nwa_toggle_power_trip ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the command (no result expected)
        serial.writeString( "\nPOWT OFF; POWT ON;\n" );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set channel coupling (couples channels together for simultaneous readings)
// COUC ON;
// COUC OFF;
// COUC?;      // Returns 1 or 0 if it's on or off
bool lars::network_analyzer::nwa_set_channel_coupling ( bool couple_channels )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Choose the appropriate command
        if ( couple_channels )
            // Send command
            serial.writeString( "\nCOUC ON;\n" );
        else
            // Send command
            serial.writeString( "\nCOUC OFF;\n" );

        // Verify Integrity

        // Request the current setting from the NWA
        serial.writeString( "\nCOUC?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != couple_channels )
            throw timeout_exception( "Setting the NWA coupling mode failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set dual (true) or single (false) display output and channel collection
// DUAC ON;
// DUAC OFF;
// DUAC?; // Returns a 1 or 0 indicating if it's on or off
bool lars::network_analyzer::nwa_set_dual_display ( bool dual_display )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Choose the appropriate command
        if ( dual_display )
            // Send command
            serial.writeString( "\nDUAC ON;\n" );
        else
            // Send command
            serial.writeString( "\nDUAC OFF;\n" );

        // Verify Integrity

        // Request the current setting from the NWA
        serial.writeString( "\nDUAC?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != dual_display )
            throw timeout_exception( "Setting the NWA dual_display mode failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Blank (true) or show (false) the display screen on the NWA
// BLAD OFF; // Show the screen
// BLAD ON; // Blank the screen
// BLAD?; // Returns a 1 or 0 indicating if it's on or off
bool lars::network_analyzer::nwa_blank_screen ( bool blank_screen )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Choose the appropriate command
        if ( blank_screen )
            // Send command
            serial.writeString( "\nBLAD ON;\n" );
        else
            // Send command
            serial.writeString( "\nBLAD OFF;\n" );

        // Verify Integrity

        // Request the current setting from the NWA
        serial.writeString( "\nBLAD?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != blank_screen )
            throw timeout_exception( "Setting the NWA blank_screen mode failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};










// Specifies which channel should be made current
bool lars::network_analyzer::nwa_set_current_channel ( int channel )
{
    try
    {
        // Validate Input
        if ( ( channel < 1 ) or (channel > 2 ) )
            throw timeout_exception( "Desired channel is out of range [1, 2]." );

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Determine which command to send
        switch ( channel )
        {
        case 1:
            // Make channel 1 active (A)
            serial.writeString( "\nOPC?; CHAN1;\n" );
            break;
        case 2:
            // Make channel 2 active (B)
            serial.writeString( "\nOPC?; CHAN2;\n" );
            break;
        default:
            throw timeout_exception( "Desired channel is out of range [1, 2]." );
        }

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != 1 )
            throw timeout_exception( "Setting the active channel on the NWA failed." );

        // Specify we'd like to perform A/R or B/R measurements
        switch ( channel )
        {
        case 1:
            // Perform A/R
            serial.writeString( "\nAR;\n" );
            break;
        case 2:
            // Perform B/R
            serial.writeString( "\nBR;\n" );
            break;
        default:
            throw timeout_exception( "Desired channel is out of range [1, 2]." );
        }

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Set the current channel to polar, form 4
bool lars::network_analyzer::nwa_set_mode_polar ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Set the current channel to polar, form 4
        serial.writeString( "\nPOLA; FORM4;\n" );

        // Wait for the action to complete
        serial.writeString( "\nOPC?; WAIT;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != 1 )
            throw timeout_exception( "Setting the active channel on the NWA to polar failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};

// Autoscale the curent display
bool lars::network_analyzer::nwa_autoscale_display ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Set the current channel to auto-scaled mode
        serial.writeString( "\nAUTO;\n" );

        // Wait for the action to complete
        serial.writeString( "\nOPC?; WAIT;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != 1 )
            throw timeout_exception( "Setting the active channel on the NWA to autoscale failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};


// Enable or Disable Averaging Mode on the NWA
// AVERO OFF; // Averaging is Off
// AVERO ON; // Averaging is On
// AVERO?; // Indicates if averaging is on (1) or off (0)
bool lars::network_analyzer::nwa_set_averaging ( bool averaging_enabled )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Choose the appropriate command
        if ( averaging_enabled )
            // Send command
            serial.writeString( "\nAVERO ON;\n" );
        else
            // Send command
            serial.writeString( "\nAVERO OFF;\n" );

        // Verify Integrity

        // Request the current setting from the NWA
        serial.writeString( "\nAVERO?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        if ( std::stoi( nwa_string ) != averaging_enabled )
            throw timeout_exception( "Setting the NWA average mode failed." );

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};


// Run a sweep of the NWA
int lars::network_analyzer::nwa_run_sweep ( lars::sweep &nwa_sweep, lars::fpga &fpga_device )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // -- Determine the number of points

        // Request the current setting from the NWA
        serial.writeString( "\nPOIN?;\n" );

        // Capture result string
        std::string nwa_string = serial.readStringUntil( "\n" );

        // Verify we've completed presets
        nwa_sweep.nwa_points = ( int ) std::stof( nwa_string );
        if ( ( nwa_sweep.nwa_points < 3 ) or (nwa_sweep.nwa_points > 1024 ) )
            throw timeout_exception( "Points retrieved by NWA is out of range." );

        // Prepare the destination and frequency pointer
        nwa_sweep.nwa_tx_off.channel_A.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_off.channel_B.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_off.frequency.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_calibrate.channel_A.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_calibrate.channel_B.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_calibrate.frequency.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_horizontal.channel_A.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_horizontal.channel_B.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_horizontal.frequency.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_vertical.channel_A.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_vertical.channel_B.resize( nwa_sweep.nwa_points );
        nwa_sweep.nwa_tx_vertical.frequency.resize( nwa_sweep.nwa_points );

        // Request the current starting frequency from the NWA
        serial.writeString( "\nSTAR?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Store the NWA setting and verify it's integrity
        nwa_sweep.nwa_start_freq = std::stof( nwa_string );
        if ( ( nwa_sweep.nwa_start_freq < 30.0e3f ) or (nwa_sweep.nwa_start_freq > 3.0e9f ) )
            throw timeout_exception( "The NWA reported a starting frequency out of range [30.0e3, 3.0e9]." );

        // Request the current starting frequency from the NWA
        serial.writeString( "\nSTOP?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Store the NWA setting and verify it's integrity
        nwa_sweep.nwa_stop_freq = std::stof( nwa_string );
        if ( ( nwa_sweep.nwa_stop_freq < 30.0e3f ) or (nwa_sweep.nwa_stop_freq > 3.0e9f ) )
            throw timeout_exception( "The NWA reported a stopping frequency out of range [30.0e3, 3.0e9]." );

        // Request the current starting frequency from the NWA
        serial.writeString( "\nPOWE?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Store the NWA setting and verify it's integrity
        nwa_sweep.nwa_output_power = std::stof( nwa_string );
        if ( ( nwa_sweep.nwa_output_power < -10.0f ) or (nwa_sweep.nwa_output_power > 10.0f ) )
            throw timeout_exception( "The NWA reported an output power out of range [30.0e3, 3.0e9]." );

        // Request the current sweep time from the NWA
        serial.writeString( "\nSWET?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Store the NWA setting and verify it's integrity
        nwa_sweep.nwa_sweep_time = std::stof( nwa_string );
        if ( ( nwa_sweep.nwa_sweep_time < 0.0f ) or (nwa_sweep.nwa_sweep_time > 10.0f ) )
            throw timeout_exception( "The NWA reported a sweep time out of range." );

        // Request the current span from the NWA
        serial.writeString( "\nSPAN?;\n" );

        // Capture result string
        nwa_string.assign( serial.readStringUntil( "\n" ) );

        // Store the NWA setting and verify it's integrity
        nwa_sweep.nwa_span_freq = std::stof( nwa_string );
        if ( ( nwa_sweep.nwa_span_freq < 0.0f ) or (nwa_sweep.nwa_span_freq > 3.0e9f ) )
            throw timeout_exception( "The NWA reported a span out of range [0.0e3, 3.0e9]." );

        // Calculate the Frequency Increment
        float f_inc = nwa_sweep.nwa_span_freq / ( nwa_sweep.nwa_points - 1 );

        // Cycle through each transmit mode
        for ( int transmit_mode = 0; transmit_mode < 4; transmit_mode++ )
        {
            // Set the current transmit mode
            if ( not fpga_device.set_tx_direction( transmit_mode ) )
                throw timeout_exception( "Unable to set transmit mode. Aborting scan." );

            // Store the timestamp for the collection
            switch ( transmit_mode )
            {
            case 0:
                nwa_sweep.nwa_tx_off.timestamp.assign( stamp( ) );
                break;
            case 1:
                nwa_sweep.nwa_tx_calibrate.timestamp.assign( stamp( ) );
                break;
            case 2:
                nwa_sweep.nwa_tx_horizontal.timestamp.assign( stamp( ) );
                break;
            case 3:
                nwa_sweep.nwa_tx_vertical.timestamp.assign( stamp( ) );
                break;
            default:
                throw timeout_exception( "transmit_mode is out of range." );
                break;
            }

            // Run a single sweep
            serial.writeString( "\nOPC?; SING;\n" );

            // Capture result string
            nwa_string.assign( serial.readStringUntil( "\n" ) );

            // Verify we've completed presets
            if ( std::stoi( nwa_string ) != 1 )
                throw timeout_exception( "Running a single sweep on the NWA failed." );


            // Store Channel 1 and 2
            for ( int current_channel = 1; current_channel < 3; current_channel++ )
            {
                // Set the current channel (this will close the serial port)
                nwa_set_current_channel( current_channel );

                // Setup Destination and Frequency Vectors
                std::complex<float> *destination;
                float *frequency;

                // Determine Destination
                switch ( transmit_mode )
                {
                case 0:
                    if ( current_channel == 1 )
                    {
                        destination = nwa_sweep.nwa_tx_off.channel_A.data( );
                        frequency = nwa_sweep.nwa_tx_off.frequency.data( );
                    }
                    else
                    {
                        destination = nwa_sweep.nwa_tx_off.channel_B.data( );
                        frequency = nwa_sweep.nwa_tx_off.frequency.data( );
                    }
                    break;
                case 1:
                    if ( current_channel == 1 )
                    {
                        destination = nwa_sweep.nwa_tx_calibrate.channel_A.data( );
                        frequency = nwa_sweep.nwa_tx_calibrate.frequency.data( );
                    }
                    else
                    {
                        destination = nwa_sweep.nwa_tx_calibrate.channel_B.data( );
                        frequency = nwa_sweep.nwa_tx_calibrate.frequency.data( );
                    }
                    break;
                case 2:
                    if ( current_channel == 1 )
                    {
                        destination = nwa_sweep.nwa_tx_horizontal.channel_A.data( );
                        frequency = nwa_sweep.nwa_tx_horizontal.frequency.data( );
                    }
                    else
                    {
                        destination = nwa_sweep.nwa_tx_horizontal.channel_B.data( );
                        frequency = nwa_sweep.nwa_tx_horizontal.frequency.data( );
                    }
                    break;
                case 3:
                    if ( current_channel == 1 )
                    {
                        destination = nwa_sweep.nwa_tx_vertical.channel_A.data( );
                        frequency = nwa_sweep.nwa_tx_vertical.frequency.data( );
                    }
                    else
                    {
                        destination = nwa_sweep.nwa_tx_vertical.channel_B.data( );
                        frequency = nwa_sweep.nwa_tx_vertical.frequency.data( );
                    }
                    break;
                default:
                    throw timeout_exception( "transmit_mode is out of range." );
                    break;
                }

                // Open the serial port if it's closed (we closed it in nwa_set_current_channel)
                if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

                // Output the sweep data
                serial.writeString( "\nFORM4; OUTPDATA;\n" );

                // Retrieve all sweep points
                for ( int i = 0; i < nwa_sweep.nwa_points; i++ )
                {
                    // Grab a single point
                    nwa_string.assign( serial.readStringUntil( "\n" ) );

                    // Store complex value to destination array
                    std::istringstream is( '(' + nwa_string + ')' );
                    is >> destination[i];

                    // Store the frequency of the point
                    frequency[i] = nwa_sweep.nwa_start_freq + ( i * f_inc );
                }
            }
        }

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return number of collected points
        return nwa_sweep.nwa_points;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
};
















































































































// Print the firmware version of the Prologix Controller
bool lars::network_analyzer::gpib_print_version ( )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Request the device's version information
        serial.writeString( "++ver\n" );

        // Print the response
        std::cout << stamp( ) << " [" << dev_name << "] " << serial.readStringUntil( "\r\n" ) << std::endl;

        // Close the serial port if it's closed
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Perform a Power-On Reset of the Controller, Sleep for 6s
bool lars::network_analyzer::gpib_reset_controller ( )
{
    try
    {
        // Update the user on status
        std::cout << stamp( ) << " [" << dev_name << "] Performing a power-on reset of the controller." << std::endl;

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Send the reset command
        serial.writeString( "++rst\n" );

        // Wait for the device to fully reset
        boost::this_thread::sleep( boost::posix_time::milliseconds( 5000 ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;

    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Enable Configuration Saving in EPROM
//   0 = Disable Saving Configuration in EPROM
//   1 = Enable Saving Configuration in EPROM
bool lars::network_analyzer::gpib_set_save_configuration ( bool gpib_config )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        if ( gpib_config == true )
            serial.writeString( "++savecfg 1\n" );
        else
            serial.writeString( "++savecfg 0\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Set Device Mode
//   0 = CONTROLLER Mode
//   1 = DEVICE Mode
bool lars::network_analyzer::gpib_set_mode ( int gpib_mode )
{
    try
    {
        std::string device_mode
                = "++mode " + std::to_string( gpib_mode ) + "\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        serial.writeString( device_mode );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Set Auto (Read after Write)
//   0 = Disable Listen-After-Write
//   1 = Enable Listen-After-Write
bool lars::network_analyzer::gpib_set_auto ( bool gpib_auto )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        if ( gpib_auto == true )
            serial.writeString( "++auto 1\n" );
        else
            serial.writeString( "++auto 0\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Set Device Address (NA = 16)
bool lars::network_analyzer::gpib_set_address ( int gpib_address )
{
    try
    {
        std::string device_address
                = "++addr " + std::to_string( gpib_address ) + "\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        serial.writeString( device_address );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Set the read and poll timeout (in milliseconds))
bool lars::network_analyzer::gpib_set_timeout ( int gpib_milliseconds )
{
    try
    {
        std::string device_timeout
                = "++read_tmo_ms " + std::to_string( gpib_milliseconds ) + "\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        serial.writeString( device_timeout );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Set EOS Character (Use 1)
//   0 = Append CR+LF
//   1 = Append CR
//   2 = Append LF
//   3 = Do not append any character
bool lars::network_analyzer::gpib_set_eos ( int gpib_eos )
{
    try
    {
        std::string device_eos
                = "++eos " + std::to_string( gpib_eos ) + "\n";

        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        serial.writeString( device_eos );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};


// Set whether the EOI character should be added
bool lars::network_analyzer::gpib_set_eoi ( bool gpib_eoi )
{
    try
    {
        // Open the serial port if it's closed
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        if ( gpib_eoi == true )
            serial.writeString( "++eoi 1\n" );
        else
            serial.writeString( "++eoi 0\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};
