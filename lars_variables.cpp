/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_variables.cpp
 * Author: David
 *
 * Created on March 30, 2016, 1:27 PM
 */

#include "lars_variables.h"

// Initialize Device Address Assignments
lars::tty lars::tty_NWA = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_A = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_B = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_C = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_D = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_E = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_F = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_G = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_H = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_I = { std::string( ), std::string( ), std::string( ), 0 };
lars::tty lars::tty_J = { std::string( ), std::string( ), std::string( ), 0 };

// Assign Azimuth Minimum Angle
int lars::azim_min_angle = 180;
int lars::azim_max_angle = 180;
int lars::azim_start_angle = 180;
int lars::azim_rest_angle = 180;
int lars::azim_tolerance = 1;
int lars::azim_timeout = 60;

// Output a string timestamp
// Ex: "YYYY-MM-DD-hh-mm-ss-nnnnnnnnn" (time in 24-hour, UTC format)
const std::string lars::stamp ( )
{
    // Build response string
    std::string system_time_string = GetStdoutFromCommand( "date -u +%Y-%m-%d-%H-%M-%S-%N" );

    // Remove the newline ending
    system_time_string.erase( std::remove( system_time_string.begin( ), system_time_string.end( ), '\n' ), system_time_string.end( ) );

    // Return string
    return system_time_string;
}


// Output a string timestamp of the date only
// Ex: "YYYY-MM-DD"
const std::string lars::stamp_date_only ( )
{
    // Build response string
    std::string system_time_string = GetStdoutFromCommand( "date -u +%Y-%m-%d" );

    // Remove the newline ending
    system_time_string.erase( std::remove( system_time_string.begin( ), system_time_string.end( ), '\n' ), system_time_string.end( ) );

    // Return string
    return system_time_string;
}

// Reset the TTY Controller
bool lars::reset_usb_adapter ( tty ttyDevice )
{
    try
    {
        // Send notification that we're resetting the USB device
        std::cout
                << stamp( )
                << " [" << ttyDevice.name
                << "] Resetting USB adapter." << std::endl;

        // Test if the device is present
        if ( access( ttyDevice.address.c_str( ), F_OK ) == -1 )
        {
            // File or device address does not exists
            std::cout << stamp( ) << " [" << ttyDevice.name << "] "
                    << "Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "no device present to reset." << std::endl;
            return false;
        }

        // Obtain the device number
        std::string deviceRequestString = "udevadm info -a -n " + ttyDevice.address + " | grep 'ATTRS{devnum}==' | grep -m 1 -o -E '[0-9]+'";
        int deviceNumber = std::stoi( GetStdoutFromCommand( deviceRequestString ) );

        // Obtain the bus number
        std::string busRequestString = "udevadm info -a -n " + ttyDevice.address + " | grep 'ATTRS{busnum}==' | grep -m 1 -o -E '[0-9]+'";
        int busNumber = std::stoi( GetStdoutFromCommand( busRequestString ) );

        // Build the device file address
        char deviceAddress[25];
        sprintf( deviceAddress, "/dev/bus/usb/%03d/%03d", busNumber, deviceNumber );


        // Open the device file address
        int fd = open( deviceAddress, O_WRONLY );
        if ( fd < 0 )
        {
            perror( "Error opening output file" );
            return false;
        }

        // Force reset the device
        int rc = ioctl( fd, USBDEVFS_RESET, 0 );
        if ( rc < 0 )
        {
            perror( "Error in ioctl" );
            return false;
        }

        // Close the file address
        close( fd );

        // Sleep for 1 second (wait for device to come back)
        boost::this_thread::sleep( boost::posix_time::milliseconds( 1000 ) );

        // Reset Complete
        std::cout
                << stamp( )
                << " [" << ttyDevice.name
                << "] Reset successful." << std::endl;

        // Return success
        return true;
    }
    catch ( boost::system::system_error& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [" << ttyDevice.name << "] Error: " << e.what( ) << std::endl;
        return false;
    }
}

// Read output from the console after issueng a command
std::string lars::GetStdoutFromCommand ( const std::string& cmd )
{
    std::string command = cmd;
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    command.append( " 2>&1" );

    stream = popen( command.c_str( ), "r" );
    if ( stream )
    {
        while ( !feof( stream ) )
            if ( std::fgets( buffer, max_buffer, stream ) != NULL ) data.append( buffer );
        pclose( stream );
    }
    return data;
}

// Parse Configuration File
lars::tty lars::find_tty_device ( const std::string& device_name )
{
    // Search by device name
    if ( device_name == tty_NWA.name ) return tty_NWA;
    else if ( device_name == tty_A.name ) return tty_A;
    else if ( device_name == tty_B.name ) return tty_B;
    else if ( device_name == tty_C.name ) return tty_C;
    else if ( device_name == tty_D.name ) return tty_D;
    else if ( device_name == tty_E.name ) return tty_E;
    else if ( device_name == tty_F.name ) return tty_F;
    else if ( device_name == tty_G.name ) return tty_G;
    else if ( device_name == tty_H.name ) return tty_H;
    else if ( device_name == tty_I.name ) return tty_I;
    else if ( device_name == tty_J.name ) return tty_J;
    else
    {
        // Build an empty device and log the error
        std::cout << stamp( )
                << " [FIND DEVICE] Error: could not locate a device called "
                << device_name << "." << std::endl;
        tty empty_device = { std::string( ), std::string( ), std::string( ), 0 };
        return empty_device;
    }



}

// Parse TTY Configuration File
bool lars::load_device_address_configuration ( const std::string& filename )
{
    try
    {
        // Instantiate the property tree
        boost::property_tree::ptree pt;

        // Load the INI file
        boost::property_tree::ini_parser::read_ini( filename, pt );

        // Assign Device - tty_NWA
        tty_NWA.name.assign( pt.get<std::string>( "tty_NWA.Name" ) );
        tty_NWA.address.assign( pt.get<std::string>( "tty_NWA.Address" ) );
        tty_NWA.serialNumber.assign( pt.get<std::string>( "tty_NWA.SerialNumber" ) );
        tty_NWA.baud = pt.get<int>( "tty_NWA.BaudRate" );
        if ( not tty_NWA.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_NWA.name << " to " << tty_NWA.address << ".\n";

        // Assign Device - tty_A
        tty_A.name.assign( pt.get<std::string>( "tty_A.Name" ) );
        tty_A.address.assign( pt.get<std::string>( "tty_A.Address" ) );
        tty_A.serialNumber.assign( pt.get<std::string>( "tty_A.SerialNumber" ) );
        tty_A.baud = pt.get<int>( "tty_A.BaudRate" );
        if ( not tty_A.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_A.name << " to " << tty_A.address << ".\n";

        // Assign Device - tty_B
        tty_B.name.assign( pt.get<std::string>( "tty_B.Name" ) );
        tty_B.address.assign( pt.get<std::string>( "tty_B.Address" ) );
        tty_B.serialNumber.assign( pt.get<std::string>( "tty_B.SerialNumber" ) );
        tty_B.baud = pt.get<int>( "tty_B.BaudRate" );
        if ( not tty_B.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_B.name << " to " << tty_B.address << ".\n";

        // Assign Device - tty_C
        tty_C.name.assign( pt.get<std::string>( "tty_C.Name" ) );
        tty_C.address.assign( pt.get<std::string>( "tty_C.Address" ) );
        tty_C.serialNumber.assign( pt.get<std::string>( "tty_C.SerialNumber" ) );
        tty_C.baud = pt.get<int>( "tty_C.BaudRate" );
        if ( not tty_C.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_C.name << " to " << tty_C.address << ".\n";

        // Assign Device - tty_D
        tty_D.name.assign( pt.get<std::string>( "tty_D.Name" ) );
        tty_D.address.assign( pt.get<std::string>( "tty_D.Address" ) );
        tty_D.serialNumber.assign( pt.get<std::string>( "tty_D.SerialNumber" ) );
        tty_D.baud = pt.get<int>( "tty_D.BaudRate" );
        if ( not tty_D.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_D.name << " to " << tty_D.address << ".\n";

        // Assign Device - tty_E
        tty_E.name.assign( pt.get<std::string>( "tty_E.Name" ) );
        tty_E.address.assign( pt.get<std::string>( "tty_E.Address" ) );
        tty_E.serialNumber.assign( pt.get<std::string>( "tty_E.SerialNumber" ) );
        tty_E.baud = pt.get<int>( "tty_E.BaudRate" );
        if ( not tty_E.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_E.name << " to " << tty_E.address << ".\n";

        // Assign Device - tty_F
        tty_F.name.assign( pt.get<std::string>( "tty_F.Name" ) );
        tty_F.address.assign( pt.get<std::string>( "tty_F.Address" ) );
        tty_F.serialNumber.assign( pt.get<std::string>( "tty_F.SerialNumber" ) );
        tty_F.baud = pt.get<int>( "tty_F.BaudRate" );
        if ( not tty_F.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_F.name << " to " << tty_F.address << ".\n";

        // Assign Device - tty_G
        tty_G.name.assign( pt.get<std::string>( "tty_G.Name" ) );
        tty_G.address.assign( pt.get<std::string>( "tty_G.Address" ) );
        tty_G.serialNumber.assign( pt.get<std::string>( "tty_G.SerialNumber" ) );
        tty_G.baud = pt.get<int>( "tty_G.BaudRate" );
        if ( not tty_G.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_G.name << " to " << tty_G.address << ".\n";

        // Assign Device - tty_H
        tty_H.name.assign( pt.get<std::string>( "tty_H.Name" ) );
        tty_H.address.assign( pt.get<std::string>( "tty_H.Address" ) );
        tty_H.serialNumber.assign( pt.get<std::string>( "tty_H.SerialNumber" ) );
        tty_H.baud = pt.get<int>( "tty_H.BaudRate" );
        if ( not tty_H.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_H.name << " to " << tty_H.address << ".\n";

        // Assign Device - tty_I
        tty_I.name.assign( pt.get<std::string>( "tty_I.Name" ) );
        tty_I.address.assign( pt.get<std::string>( "tty_I.Address" ) );
        tty_I.serialNumber.assign( pt.get<std::string>( "tty_I.SerialNumber" ) );
        tty_I.baud = pt.get<int>( "tty_I.BaudRate" );
        if ( not tty_I.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_I.name << " to " << tty_I.address << ".\n";

        // Assign Device - tty_J
        tty_J.name.assign( pt.get<std::string>( "tty_J.Name" ) );
        tty_J.address.assign( pt.get<std::string>( "tty_J.Address" ) );
        tty_J.serialNumber.assign( pt.get<std::string>( "tty_J.SerialNumber" ) );
        tty_J.baud = pt.get<int>( "tty_J.BaudRate" );
        if ( not tty_J.name.empty( ) ) std::cout << stamp( ) << " [CONFIG PARSER] Assigning "
            << tty_J.name << " to " << tty_J.address << ".\n";

        // Return Success
        return true;
    }
    catch ( const boost::property_tree::ptree_error &e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [CONFIG PARSER] Error: " << e.what( ) << std::endl;
        return false;
    }
}


// Parse Azimuth Configuration File
bool lars::load_azimuth_configuration ( const std::string& filename )
{
    try
    {
        // Instantiate the property tree
        boost::property_tree::ptree pt;

        // Load the INI file
        boost::property_tree::ini_parser::read_ini( filename, pt );

        // Assign Azimuth Parameters
        azim_min_angle = pt.get<int>( "AZIM.Min_Angle" );
        azim_max_angle = pt.get<int>( "AZIM.Max_Angle" );
        azim_start_angle = pt.get<int>( "AZIM.Start_Angle" );
        azim_rest_angle = pt.get<int>( "AZIM.Rest_Angle" );
        azim_tolerance = pt.get<int>( "AZIM.Offset_Tolerance" );
        azim_timeout = pt.get<int>( "AZIM.Timeout" );

        // Integrity Check

        // Check that the minimum angle is within range
        if ( ( azim_min_angle < 0 ) or (azim_min_angle > 360 ) )
            throw boost::property_tree::ptree_error( "Minimum angle out of range [0, 359]." );

        // Check that the resting angle is between the min and max
        if ( ( azim_max_angle < 0 ) or (azim_max_angle > 360 ) )
            throw boost::property_tree::ptree_error( "Maximum angle out of range [0, 359]." );

        // Check that the minimum angle is less than the maximum angle
        if ( azim_max_angle < azim_min_angle )
            throw boost::property_tree::ptree_error( "Maximum angle must be larger than the minimum angle." );

        // Check that the maximum angle is within range
        if ( ( azim_max_angle < azim_rest_angle ) or (azim_min_angle > azim_rest_angle ) )
            throw boost::property_tree::ptree_error( "Resting angle must fall between min and max angles." );

        // Check that the starting angle is within range
        if ( ( azim_max_angle < azim_start_angle ) or (azim_min_angle > azim_start_angle ) )
            throw boost::property_tree::ptree_error( "Starting angle must fall between min and max angles." );

        // Check that the offset tolerance is positive
        if ( azim_tolerance < 0 )
            throw boost::property_tree::ptree_error( "Tolerance angle must be GTE to zero." );

        // Check that the offset tolerance is positive
        if ( azim_timeout < 0 )
            throw boost::property_tree::ptree_error( "Timeout angle must be GTE to zero." );

        // Print the Minimum Angle
        std::cout << stamp( ) << " [CONFIG PARSER] Azimuth minimum angle set to: "
                << std::to_string( azim_min_angle ) << " degrees.\n";

        // Print the Maximum Angle
        std::cout << stamp( ) << " [CONFIG PARSER] Azimuth maximum angle set to: "
                << std::to_string( azim_max_angle ) << " degrees.\n";

        // Print the Resting Angle
        std::cout << stamp( ) << " [CONFIG PARSER] Azimuth resting angle set to: "
                << std::to_string( azim_rest_angle ) << " degrees.\n";

        // Print the Starting Angle
        std::cout << stamp( ) << " [CONFIG PARSER] Azimuth starting angle set to: "
                << std::to_string( azim_start_angle ) << " degrees.\n";

        // Print the Tolerance Angle
        std::cout << stamp( ) << " [CONFIG PARSER] Azimuth offset tolerance set to: "
                << std::to_string( azim_tolerance ) << " degree(s).\n";

        // Return Success
        return true;
    }
    catch ( const boost::property_tree::ptree_error &e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [CONFIG PARSER] Error: " << e.what( ) << std::endl;
        return false;
    }
}


// Write sweep to ascii file
//   Files are written as "YYYY-MM-DD-hh-mm-ss-nnnnnnnnn.txt", according to the
//   timestamp on the sweep.
bool lars::write_sweep_to_ascii_file ( sweep sweep_data, std::string folder )
{
    try
    {
        // Create the folder if it does not exist
        boost::filesystem::path p( folder.c_str( ) );
        try
        {
            if ( not boost::filesystem::is_directory( p ) )
                boost::filesystem::create_directories( p );
        }
        catch ( const boost::filesystem::filesystem_error& e )
        {
            if ( e.code( ) == boost::system::errc::permission_denied )
                std::cout << "Search permission is denied for one of the directories "
                    << "in the path prefix of " << p << "\n";
            else
                std::cout << "is_directory(" << p << ") failed with "
                    << e.code( ).message( ) << '\n';
        }

        // Build the filename
        std::string filename = folder + '/' + sweep_data.timestamp + ".txt";

        // Create the file (if it exists, overwrite it)
        std::ofstream asciiFile;
        asciiFile.open( filename.c_str( ), std::ios::out | std::ios::trunc );

        // Check if the file is accessible
        if ( not asciiFile.is_open( ) )
            throw timeout_exception( "Could not create or access write file." );

        // -- Store Timing Parameters
        asciiFile << sweep_data.timestamp << ",";
        asciiFile << sweep_data.nwa_tx_off.timestamp << ",";
        asciiFile << sweep_data.nwa_tx_calibrate.timestamp << ",";
        asciiFile << sweep_data.nwa_tx_horizontal.timestamp << ",";
        asciiFile << sweep_data.nwa_tx_vertical.timestamp << ",";

        // -- Store Azimuth Parameters
        asciiFile << std::setprecision( 8 ) << std::scientific
                << sweep_data.azimuth_current << ","
                << sweep_data.azimuth_min << ","
                << sweep_data.azimuth_max << ","
                << sweep_data.azimuth_inc << ",";

        // -- Store Inclinometer Parameters
        asciiFile << std::setprecision( 8 ) << std::scientific
                << sweep_data.inc_horn << ","
                << sweep_data.inc_back << ","
                << sweep_data.inc_front << ",";

        // -- Store Temperature Parameters
        asciiFile << std::setprecision( 8 ) << std::scientific
                << sweep_data.therm_box << ","
                << sweep_data.therm_horn << ","
                << sweep_data.therm_rf << ",";

        // -- Store FPGA Parameters
        asciiFile << std::setprecision( 8 ) << std::scientific
                << sweep_data.fpga_sysclock << ","
                << std::fixed
                << sweep_data.fpga_freq_div << ","
                << sweep_data.fpga_duty_div << ","
                << sweep_data.fpga_switches << ","
                << sweep_data.fpga_pwm << ",";

        // -- Store Network Analyzer Parameters
        asciiFile << std::setprecision( 8 ) << std::scientific
                << sweep_data.nwa_start_freq << ","
                << sweep_data.nwa_stop_freq << ","
                << sweep_data.nwa_span_freq << ","
                << sweep_data.nwa_output_power << ","
                << sweep_data.nwa_sweep_time << ","
                << std::fixed
                << sweep_data.nwa_points << std::endl;

        // -- Store Network Analyzer Channel A and B Collections
        for ( int point = 0; point < sweep_data.nwa_points; point++ )
        {
            asciiFile << point
                    << ","
                    << std::scientific

                    // Transmit Mode - OFF
                    << std::setprecision( 3 )
                    << sweep_data.nwa_tx_off.frequency[point]
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_off.channel_A[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_off.channel_A[point].imag( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_off.channel_B[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_off.channel_B[point].imag( )
                    << ","
                    // Transmit Mode - CALIBRATE
                    << std::setprecision( 3 )
                    << sweep_data.nwa_tx_calibrate.frequency[point]
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_calibrate.channel_A[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_calibrate.channel_A[point].imag( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_calibrate.channel_B[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_calibrate.channel_B[point].imag( )
                    << ","
                    // Transmit Mode - HORIZONTAL
                    << std::setprecision( 3 )
                    << sweep_data.nwa_tx_horizontal.frequency[point]
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_horizontal.channel_A[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_horizontal.channel_A[point].imag( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_horizontal.channel_B[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_horizontal.channel_B[point].imag( )
                    << ","
                    // Transmit Mode - VERTICAL
                    << std::setprecision( 3 )
                    << sweep_data.nwa_tx_vertical.frequency[point]
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_vertical.channel_A[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_vertical.channel_A[point].imag( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_vertical.channel_B[point].real( )
                    << ","
                    << std::setprecision( 8 )
                    << sweep_data.nwa_tx_vertical.channel_B[point].imag( )

                    // End Line
                    << std::endl;
        }

        // Flush the stream and close the file
        asciiFile.close( );

        // Return Success
        return true;
    }
    catch ( timeout_exception& e )
    {
        // On error, output the message
        std::cout << stamp( ) << " [SAVE_SWEEP] Error: " << e.what( ) << std::endl;
        return false;
    }
}
