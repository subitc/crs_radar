/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_fpga.cpp
 * Author: David
 *
 * Created on March 30, 2016, 1:44 PM
 */

// LARS FPGA
#include "lars_fpga.h"

// LARS FPGA Constructor
lars::fpga::fpga ( tty ttyDevice )
{
    // Declare device as uninitialized
    initialized = false;

    // Set the FPGA Clock
    sys_clock = 200.0e6;

    // Store the device's TTY
    update_tty( ttyDevice );

    // Set the timeout period
    serial.setTimeout( boost::posix_time::seconds( 5 ) );

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
                    << "port did not respond to 'open' command." << std::endl;
        }

        // Initialize Controller
        if ( initialize( ) == false )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "could not initialize the device to default settings." << std::endl;

            // Finish Initialization
            initialized = false;
        }
        else
        {
            // Finish Initialization
            initialized = true;
        }
    }
}

// LARS FPGA Destructor
lars::fpga::~fpga ( )
{
    // Print Goodbye
    std::cout << stamp( ) << " [" << dev_name << "] Closing port." << std::endl;

    // Close the serial port
    if ( serial.isOpen( ) ) serial.close( );
}

// Set the assigned TTY device
void lars::fpga::update_tty ( tty ttyDevice )
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
bool lars::fpga::is_initialized ( )
{
    return initialized;
}

// FPGA Block Initialization
bool lars::fpga::initialize ( )
{
    try
    {
        // Print Opening Message
        std::cout << stamp( ) << " [" << dev_name << "] Instantiating new device." << std::endl;

        // Set default value for register 0x00
        set_tx_direction( TX_DIR_OFF );

        // Set default value for register 0x01
        set_tx_switch( 0 );
        set_rxA_switch( 0 );
        set_rxB_switch( 0 );

        // Set default value for register 0x02
        set_pwm_tx_enabled( 1 );
        set_pwm_chA_enabled( 1 );
        set_pwm_chB_enabled( 1 );
        set_pwm_enabled( 1 );

        // Set the default range (alternative to frequency)
        set_range( 30.0f, 0.5f );

        // Finished Initialization
        std::cout << stamp( ) << " [" << dev_name << "] Instantiation complete." << std::endl;

        // return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
};

// Restarts the PWM block
bool lars::fpga::restart_pwm_block ( )
{
    try
    {
        // Enable the PWM Block
        if ( set_pwm_enabled( true ) == false )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "could not enable the PWM block." << std::endl;
            return false;
        }

        // Disable the PWM Block
        if ( set_pwm_enabled( false ) == false )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "could not disable the PWM block." << std::endl;
            return false;
        }

        // Return completed
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the pulse repetition frequency
float lars::fpga::read_frequency ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the upper and lower registers
        std::stringstream sstream;
        serial.writeString( "\r\rR 06\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 05\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned long response_int = std::stoul( sstream.str( ), nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 65535 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Determine actual frequency
        float actual_frequency = ( sys_clock / response_int );

        // Return the desired bit
        return actual_frequency;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the maximum range
float lars::fpga::read_range ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the upper and lower registers
        std::stringstream sstream;
        serial.writeString( "\r\rR 06\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 05\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned long response_int = std::stoul( sstream.str( ), nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 65535 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Determine actual frequency
        float actual_frequency = ( sys_clock / response_int );
        float actual_range = CONST_c / ( 2.0f * actual_frequency );

        // Return the desired bit
        return actual_range;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the duty
float lars::fpga::read_duty ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the upper and lower registers
        std::stringstream sstream;
        serial.writeString( "\r\rR 04\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 03\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned long pwm_duty_int = std::stoul( sstream.str( ), nullptr, 16 );

        // Check the integrity of the value
        if ( ( pwm_duty_int < 0 ) or (pwm_duty_int > 65535 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the upper and lower registers
        sstream.str( std::string( ) );
        serial.writeString( "\r\rR 06\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 05\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned long pwm_freq_div_int = std::stoul( sstream.str( ), nullptr, 16 );

        // Check the integrity of the value
        if ( ( pwm_freq_div_int < 0 ) or (pwm_freq_div_int > 65535 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Determine Fractional Duty
        float actual_duty = ( float ) pwm_freq_div_int / ( float ) pwm_freq_div_int;

        // Return the desired bit
        return actual_duty;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the output direction of tx
int lars::fpga::read_tx_direction ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 00\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 3 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return response_int;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the on/off state of chA (SW1)
bool lars::fpga::read_rxA_switch ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 01\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> SW_1 ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the on/off state of chB (SW3)
bool lars::fpga::read_rxB_switch ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 01\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> SW_3 ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the on/off state of tx (SW2)
bool lars::fpga::read_tx_switch ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 01\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> SW_2 ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns the on/off state of the PWM block
bool lars::fpga::read_pwm_enabled ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> BLK_SET ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns if chA is using the PWM
bool lars::fpga::read_pwm_chA_enabled ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> PWM_SW_1 ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns if chB is using the PWM
bool lars::fpga::read_pwm_chB_enabled ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> PWM_SW_3 ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns if tx is using the PWM
bool lars::fpga::read_pwm_tx_enabled ( )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Return the desired bit
        return(response_int >> PWM_SW_2 ) & 1;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns actual value set due to rounding
/* Input: Float (3051.9 to 100e6) indicating desired frequency in meters
 * Output: Float of actual (rounded) frequency used
 */
float lars::fpga::set_frequency ( float desired_frequency, float desired_duty )
{
    try
    {
        // Check if input is within the correct range
        if ( ( desired_frequency < 3051.9f ) or (desired_frequency > 100.0e6f ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "desired_frequency is out of range." << std::endl;
            return -1.0f;
        }

        // Check if input is within the correct range
        if ( ( desired_duty < 0.0f ) or (desired_duty > 1.0f ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "input duty is out of range." << std::endl;
            return -1.0f;
        }

        // Determine the pwm_divider for the desirec frequency
        unsigned int new_pwm_divider = std::round( sys_clock / desired_frequency );

        // Convert to a hex string
        std::stringstream sstream;
        std::string fpga_string;
        sstream << std::setfill( '0' )
                << std::setw( 4 )
                << std::hex
                << new_pwm_divider;
        std::string new_pwm_divider_H_str = sstream.str( ).substr( 0, 2 );
        std::string new_pwm_divider_L_str = sstream.str( ).substr( 2, 2 );

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Prepare and send the upper part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_pwm_divider_H_str
                << " 06\r";
        fpga_string.assign( sstream.str( ) );
        serial.writeString( fpga_string );

        // Prepare and send the lower part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_pwm_divider_L_str
                << " 05\r";
        fpga_string.assign( sstream.str( ) );
        serial.writeString( fpga_string );

        // Integrity Check

        // Read the current state of the upper and lower registers
        sstream.str( std::string( ) );
        serial.writeString( "\r\rR 06\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 05\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        fpga_string.assign( sstream.str( ) );
        unsigned long pwm_divider = std::stoul( fpga_string, nullptr, 16 );

        // Check if the register is the value we wrote
        if ( pwm_divider != new_pwm_divider )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "register holds wrong frequency value." << std::endl;
            //return -1.0f;
        }

        // return actual frequency
        float actual_frequency = float(sys_clock / pwm_divider );

        // Determine the duty value
        unsigned int new_duty = std::round( ( float ) new_pwm_divider * desired_duty );

        // Convert to a hex string
        sstream.str( std::string( ) );
        sstream << std::setfill( '0' )
                << std::setw( 4 )
                << std::hex
                << new_duty;
        std::string new_duty_H_str = sstream.str( ).substr( 0, 2 );
        std::string new_duty_L_str = sstream.str( ).substr( 2, 2 );

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Prepare and send the upper part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_duty_H_str
                << " 04\r";
        serial.writeString( sstream.str( ) );

        // Prepare and send the lower part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_duty_L_str
                << " 03\r";
        serial.writeString( sstream.str( ) );

        // Integrity Check

        // Read the current state of the upper and lower registers
        sstream.str( std::string( ) );
        serial.writeString( "\r\rR 04\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 03\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned long duty_int = std::stoul( sstream.str( ), nullptr, 16 );

        // Check if the register is the value we wrote
        if ( duty_int != new_duty )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "register holds wrong duty value." << std::endl;
            //return -1.0f;
        }

        // Restart the PWM Block and Exit
        if ( restart_pwm_block( ) )
            return actual_frequency;
        else
            return -1.0f;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Returns actual value set due to rounding
/* Input: Float (1.499 to 4.911e4) indicating desired range in meters
 * Output: Float of actual (rounded) range used
 */
float lars::fpga::set_range ( float desired_range, float desired_duty )
{
    try
    {
        // Check if input is within the correct range
        if ( ( desired_range < 1.499f ) or (desired_range > 4.911e4f ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "desired_range is out of range." << std::endl;
            return -1.0f;
        }

        // Check if input is within the correct range
        if ( ( desired_duty < 0.0f ) or (desired_duty > 1.0f ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "input duty is out of range." << std::endl;
            return -1.0f;
        }

        // Determine the desired frequency for the desired range
        float desired_frequency = CONST_c / ( 2.0f * desired_range );

        // Determine the pwm_divider for the desired frequency
        unsigned int new_pwm_divider = std::round( sys_clock / desired_frequency );

        // Convert to a hex string
        std::stringstream sstream;
        sstream << std::setfill( '0' )
                << std::setw( 4 )
                << std::hex
                << new_pwm_divider;
        std::string new_pwm_divider_H_str = sstream.str( ).substr( 0, 2 );
        std::string new_pwm_divider_L_str = sstream.str( ).substr( 2, 2 );

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Prepare and send the upper part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_pwm_divider_H_str
                << " 06\r";
        serial.writeString( sstream.str( ) );

        // Prepare and send the lower part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_pwm_divider_L_str
                << " 05\r";
        serial.writeString( sstream.str( ) );

        // Integrity Check

        // Read the current state of the upper and lower registers
        sstream.str( std::string( ) );
        serial.writeString( "\r\rR 06\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 05\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        std::string fpga_response = sstream.str( );
        unsigned long pwm_divider = std::stoul( fpga_response, nullptr, 16 );

        // Check if the register is the value we wrote
        if ( pwm_divider != new_pwm_divider )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "register holds wrong value." << std::endl;
            //return -1.0f;
        }

        // Determine actual range
        float actual_frequency = float(sys_clock / pwm_divider );
        float actual_range = CONST_c / ( 2.0f * actual_frequency );

        // Determine the duty value
        unsigned int new_duty = std::round( ( float ) new_pwm_divider * desired_duty );

        // Convert to a hex string
        sstream.str( std::string( ) );
        sstream << std::setfill( '0' )
                << std::setw( 4 )
                << std::hex
                << new_duty;
        std::string new_duty_H_str = sstream.str( ).substr( 0, 2 );
        std::string new_duty_L_str = sstream.str( ).substr( 2, 2 );

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Prepare and send the upper part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_duty_H_str
                << " 04\r";
        serial.writeString( sstream.str( ) );

        // Prepare and send the lower part
        sstream.str( std::string( ) );
        sstream << "\r\rW "
                << new_duty_L_str
                << " 03\r";
        serial.writeString( sstream.str( ) );

        // Integrity Check

        // Read the current state of the upper and lower registers
        sstream.str( std::string( ) );
        serial.writeString( "\r\rR 04\r" );
        sstream << serial.readStringUntil( "\r\n" );
        serial.writeString( "\r\rR 03\r" );
        sstream << serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned long duty_int = std::stoul( sstream.str( ), nullptr, 16 );

        // Check if the register is the value we wrote
        if ( duty_int != new_duty )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "register holds wrong value." << std::endl;
            return -1.0f;
        }

        // Restart the PWM Block and Exit
        if ( restart_pwm_block( ) )
            return actual_range;
        else
            return -1.0f;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return -1.0f;
    }
}

//// Set PWM Duty Cycle
///* Input: Float (0, 1) indicating percentage duty
// * Output: True if the set was a success
// */
//bool lars::fpga::set_duty (float duty)
//{
//    try
//    {
//        // Check if input is within the correct range
//        if ((duty < 0.0f) or (duty > 1.0f))
//        {
//            std::cout << stamp()
//                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
//                    << "input duty is out of range." << std::endl;
//            return false;
//        }
//
//        // Open the serial port
//        if (not serial.isOpen()) serial.open(dev_address, dev_baud);
//
//        // Read the current state of the upper and lower registers
//        std::stringstream sstream;
//        serial.writeString("\r\rR 06\r");
//        sstream << serial.readStringUntil("\r\n");
//        serial.writeString("\r\rR 05\r");
//        sstream << serial.readStringUntil("\r\n");
//
//        // Close the serial port
//        if (serial.isOpen()) serial.close();
//
//        // Convert the hex response to an unsigned int
//        unsigned long freq_divide_int = std::stoul(sstream.str(), nullptr, 16);
//
//        // Check the integrity of the value
//        if ((freq_divide_int < 0) or (freq_divide_int > 65535))
//        {
//            std::cout << stamp()
//                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
//                    << "freq_divide_int out of bounds." << std::endl;
//            return false;
//        }
//
//        // Determine the duty value
//        unsigned int new_duty = std::round((float) freq_divide_int * duty);
//
//        // Convert to a hex string
//        sstream.str(std::string());
//        sstream << std::setfill('0')
//                << std::setw(4)
//                << std::hex
//                << new_duty;
//        std::string new_duty_H_str = sstream.str().substr(0, 2);
//        std::string new_duty_L_str = sstream.str().substr(2, 2);
//
//        // Open the serial port
//        if (not serial.isOpen()) serial.open(dev_address, dev_baud);
//
//        // Prepare and send the upper part
//        sstream.str(std::string());
//        sstream << "\r\rW "
//                << new_duty_H_str
//                << " 04\r";
//        serial.writeString(sstream.str());
//
//        // Prepare and send the lower part
//        sstream.str(std::string());
//        sstream << "\r\rW "
//                << new_duty_L_str
//                << " 03\r";
//        serial.writeString(sstream.str());
//
//        // Integrity Check
//
//        // Read the current state of the upper and lower registers
//        sstream.str(std::string());
//        serial.writeString("\r\rR 04\r");
//        sstream << serial.readStringUntil("\r\n");
//        serial.writeString("\r\rR 03\r");
//        sstream << serial.readStringUntil("\r\n");
//
//        // Close the serial port
//        if (serial.isOpen()) serial.close();
//
//        // Convert the hex response to an unsigned int
//        unsigned long duty_int = std::stoul(sstream.str(), nullptr, 16);
//
//        // Check if the register is the value we wrote
//        if (duty_int != new_duty)
//        {
//            std::cout << stamp()
//                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
//                    << "register holds wrong value." << std::endl;
//            return false;
//        }
//
//        // Restart the PWM Block and Exit
//        if (restart_pwm_block())
//            return true;
//        else
//            return false;
//    }
//    catch (boost::system::system_error& e)
//    {
//        // On error, output the message
//        std::cout << stamp()
//                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
//                << e.what() << std::endl;
//        return false;
//    }
//}

// Sets the output direction of the tx line
/* Input: Int (0,1,2,3) desired TX output path
 * Output: True if the set was a success
 */
bool lars::fpga::set_tx_direction ( int direction )
{
    try
    {
        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW ";

        // Choose the output channel
        switch ( direction )
        {
        case TX_DIR_OFF: // Disconnect transmit path
            sstream << "00 ";
            break;

        case TX_DIR_CALIBRATE: // Route transmit to calibrate
            sstream << "01 ";
            break;

        case TX_DIR_HORIZONTAL: // Route transmit to horizontal
            sstream << "02 ";
            break;

        case TX_DIR_VERTICAL: // Route transmit to vertical
            sstream << "03 ";
            break;

        default:
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "direction out of bounds." << std::endl;
            return false;
            break;
        }

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Add the register and send the write string
        sstream << "00\r";
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_tx_direction( ) == direction )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets SW1 path
/* Input: Bool to enable(true) or disable(false) switch path
 * Output: True if the set was a success
 */
bool lars::fpga::set_rxA_switch ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 01\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << SW_1; // Set the bit
        else
            response_int &= ~( 1 << SW_1 ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 01\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_rxA_switch( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets SW3 path
/* Input: Bool to enable(true) or disable(false) switch path
 * Output: True if the set was a success
 */
bool lars::fpga::set_rxB_switch ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 01\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << SW_3; // Set the bit
        else
            response_int &= ~( 1 << SW_3 ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 01\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_rxB_switch( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets SW2 path
/* Input: Bool to enable(true) or disable(false) switch path
 * Output: True if the set was a success
 */
bool lars::fpga::set_tx_switch ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 01\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << SW_2; // Set the bit
        else
            response_int &= ~( 1 << SW_2 ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 01\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_tx_switch( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets if the PWM block is en(1) or dis(0)
/* Input: Bool to enable(true) or disable(false) the PWM block
 * Output: True if the set was a success
 */
bool lars::fpga::set_pwm_enabled ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << BLK_SET; // Set the bit
        else
            response_int &= ~( 1 << BLK_SET ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 02\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        std::string sstream_str = sstream.str( );
        serial.writeString( sstream_str );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_pwm_enabled( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets chA is using the PWM
/* Input: Bool: true=(use PWM) | false=(use manual control)
 * Output: True if the set was a success
 */
bool lars::fpga::set_pwm_chA_enabled ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << PWM_SW_1; // Set the bit
        else
            response_int &= ~( 1 << PWM_SW_1 ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 02\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_pwm_chA_enabled( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets chB is using the PWM
/* Input: Bool: true=(use PWM) | false=(use manual control)
 * Output: True if the set was a success
 */
bool lars::fpga::set_pwm_chB_enabled ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << PWM_SW_3; // Set the bit
        else
            response_int &= ~( 1 << PWM_SW_3 ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 02\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_pwm_chB_enabled( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}

// Sets tx is using the PWM
/* Input: Bool: true=(use PWM) | false=(use manual control)
 * Output: True if the set was a success
 */
bool lars::fpga::set_pwm_tx_enabled ( bool enable )
{
    try
    {
        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Read the current state of the register
        serial.writeString( "\r\rR 02\r" );
        std::string response_str = serial.readStringUntil( "\r\n" );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Convert the hex response to an unsigned int
        unsigned int response_int = std::stoi( response_str, nullptr, 16 );

        // Check the integrity of the value
        if ( ( response_int < 0 ) or (response_int > 255 ) )
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "response out of bounds." << std::endl;
            return false;
        }

        // Set or clear the desired bit
        if ( enable )
            response_int |= 1 << PWM_SW_2; // Set the bit
        else
            response_int &= ~( 1 << PWM_SW_2 ); // Clear the bit

        // Prepare the output string
        std::stringstream sstream;
        sstream << "\r\rW "
                << std::setfill( '0' )
                << std::setw( 2 )
                << std::hex
                << response_int
                << " 02\r";

        // Open the serial port
        if ( not serial.isOpen( ) ) serial.open( dev_address, dev_baud );

        // Write to the register
        serial.writeString( sstream.str( ) );

        // Close the serial port
        if ( serial.isOpen( ) ) serial.close( );

        // Integrity Check
        if ( read_pwm_tx_enabled( ) == enable )
            return true;
        else
        {
            std::cout << stamp( )
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "integrity check failed at " << __func__ << "." << std::endl;
            return false;
        }
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( )
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what( ) << std::endl;
        return false;
    }
}