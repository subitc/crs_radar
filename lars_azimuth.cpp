/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_azimuth.cpp
 * Author: David
 *
 */

//LARS Azimuth Controller
#include "lars_azimuth.h"

// LARS Azimuth Constructor
lars::azimuth::azimuth (tty ttyDevice)
{
    // Declare device as uninitialized
    initialized = false;

    // Store the device's TTY
    update_tty (ttyDevice);

    // Set the timeout period
    serial.setTimeout (boost::posix_time::seconds (5));

    // Reset the Serial Adapter
    if (reset_usb_adapter (ttyDevice))
    {

        // Test the serial port
        serial.open (dev_address, dev_baud);
        if (serial.isOpen ())
        {
            // Port opened - close and report it's functional
            serial.close ();
            std::cout << stamp () << " [" << dev_name << "] Serial port is functional." << std::endl;
        }
        else
        {
            // Port wasn't open - report it's not responding to our command
            std::cout << stamp ()
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "port did not respond to 'open' command." << std::endl;
        }

        // Initialize Controller
        if (initialize () == false)
        {
            std::cout << stamp ()
                    << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                    << "could not initialize the device to default settings." << std::endl;
        }

        // Finish Initialization
        initialized = true;
    }
}

// LARS Azimuth Destructor
lars::azimuth::~azimuth ()
{
    // Print Goodbye
    std::cout << stamp () << " [" << dev_name << "] Closing port." << std::endl;

    // Close the serial port
    if (serial.isOpen ()) serial.close ();
}

// Set the assigned TTY device
void lars::azimuth::update_tty (tty ttyDevice)
{
    // Store the device's name
    dev_name.assign (ttyDevice.name);

    // Store the device's address
    dev_address.assign (ttyDevice.address);

    // Store the device's serial number
    dev_serialnumber.assign (ttyDevice.serialNumber);

    // Store the device's baud rate
    dev_baud = ttyDevice.baud;
}

// Check if the device was initialized
bool lars::azimuth::is_initialized ()
{
    return initialized;
}

// Azimuth Block Initialization
bool lars::azimuth::initialize ()
{
    try
    {
        // Print Opening Message
        std::cout << stamp () << " [" << dev_name << "] Instantiating new device." << std::endl;

        /* TO DO - FILL INITIALIZATION */

        // Finished Initialization
        std::cout << stamp () << " [" << dev_name << "] Instantiation complete." << std::endl;

        // return success
        return true;
    }
    catch (boost::system::system_error& e)
    {
        // On error, output the message
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what () << std::endl;
        return false;
    }
};

// Run Command
// If Expecting Data: Output will be data value, failure(-1)
// If Not Expecting Data: Output data will be success(1) or failure(-1)
int lars::azimuth::run_command (bool expect_result, const std::string& command_string)
{
    try
    {
        // Setup location for result
        int result_int = -1;

        // Open the serial port
        if (not serial.isOpen ()) serial.open (dev_address, dev_baud);
        
        // Clear out the last command and buffer
        serial.writeString("\x0D");
        serial.readStringUntil ("\n");
        serial.clearBuffer();   
        
        // Send the desired command
        serial.writeString (command_string);

        // Are we expecting incoming data?
        if (expect_result)
        {
            // Read the return data
            std::string result_string = serial.readStringUntil ("\n");
            
            // Convert the result to an integer
            result_int = std::stoi (result_string.substr(3,3));
        }
        else
        {
            // Set result to success
            result_int = 1;
        }

        // Close the serial port
        if (serial.isOpen ()) serial.close ();

        // Return success
        return result_int;

    }
    catch (boost::system::system_error& e)
    {
        // On error, output the message
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what () << std::endl;
        return -1;
    }
    catch (timeout_exception& e)
    {
        // On error, output the message
        std::cout << stamp ()
                << " [" << dev_name << "] Timeout error in " << __FILE__ << "(" << __LINE__ << "): "
                << e.what () << std::endl;
        return -1;
    }
}



// Set Manual Rotation
bool lars::azimuth::manual_rotate (lars::azimuth::rotate_mode mode, int stop_after_seconds)
{
    // Setup variable to store command success/failure result
    int run_result_int = -1;

    // Execute the desired motion command
    switch (mode)
      {
      case rotate_mode::ROTATE_CLOCKWISE:
        // Send clockwise rotation command
        run_result_int = run_command (false, "R\x0D");
        break;
      case rotate_mode::ROTATE_COUNTER_CLOCKWISE:
        // Send clockwise rotation command
        run_result_int = run_command (false, "L\x0D");
        break;
      case rotate_mode::ROTATE_STOP:
        // Send clockwise rotation command
        run_result_int = run_command (false, "A\x0D");
        break;
      default:
        // The mode selected is unknown
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "invalid mode; see rotate_mode for a list of options." << std::endl;
        return false;
      }

    // Execute Motion Command
    if (run_result_int == -1)
    {
        // Notify the user there was an error
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "manual motion command returned failure." << std::endl;
        return false;
    }

    // Stop after indicated time (if asked by the user)
    if (stop_after_seconds > 0)
    {
        // Sleep for given amount of seconds
        boost::this_thread::sleep (boost::posix_time::milliseconds(1000 * stop_after_seconds));

        // Send a stop command to the motor
        manual_rotate (rotate_mode::ROTATE_STOP);
    }

    // Return success
    return true;
}

// Set Rotation Speed
bool lars::azimuth::set_rotation_speed (lars::azimuth::speed_mode mode)
{
    // Setup variable to store command success/failure result
    int run_result_int = -1;

    // Execute the desired speed command
    switch (mode)
      {
      case speed_mode::SPEED_STAGNANT:
        // Set speed to SPEED_STAGNANT
        run_result_int = run_command (false, "X1\x0D");
        break;
      case speed_mode::SPEED_SLOW:
        // Set speed to SPEED_SLOW
        run_result_int = run_command (false, "X2\x0D");
        break;
      case speed_mode::SPEED_MEDIUM:
        // Set speed to SPEED_MEDIUM
        run_result_int = run_command (false, "X3\x0D");
        break;
      case speed_mode::SPEED_FAST:
        // Set speed to SPEED_FAST
        run_result_int = run_command (false, "X4\x0D");
        break;
      default:
        // The mode selected is unknown
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "invalid mode; see speed_mode for a list of options." << std::endl;
        return false;
      }

    // Execute Motion Command
    if (run_result_int == -1)
    {
        // Notify the user there was an error
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "setting speed mode returned failure." << std::endl;
        return false;
    }

    // Return success
    return true;
}

// Read Azimuth Value
int lars::azimuth::value ()
{
    // Ask the controller for the current value
    int run_result_int = run_command (true, "C\x0D");

    // Verify it is within bounds
    if ((run_result_int < 0) or (run_result_int > 360))
    {
        // Notify the user there was an error
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "read azimuth value our of [0,360] degree range: ("
                << std::to_string (run_result_int) << ")." << std::endl;
        run_result_int = -1;
    }

    // Return the read value
    return run_result_int;
}

// Set Azimuth Value
bool lars::azimuth::value (int desired_azimuth)
{
    // Verify it is within bounds
    if ((desired_azimuth < azim_min_angle) or (desired_azimuth > azim_max_angle))
    {
        // Notify the user there was an error
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "desired azimuth is out of bounds." << std::endl;
        return false;
    }

    // Build the command
    std::stringstream sstream;
    sstream << "M"
            << std::setfill ('0')
            << std::setw (3)
            << std::to_string (desired_azimuth)
            << "\x0D";

    // Ask the controller for the current value
    int run_result_int = run_command (false, sstream.str ());

    // Execute Motion Command
    if (run_result_int == -1)
    {
        // Notify the user there was an error
        std::cout << stamp ()
                << " [" << dev_name << "] Error in " << __FILE__ << "(" << __LINE__ << "): "
                << "setting the azimuth returned an error." << std::endl;
        return false;
    }
    
    // Wait until the azimuth controller arrives at it's destination
    int current_azimuth = value();
    int timeout = azim_timeout;
    while ((abs(current_azimuth - desired_azimuth) > azim_tolerance ) and (timeout > 0))
    {        
        // Sleep for 1 second (we're probably still moving...)
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        
        // Check the current azimuth
        int temp_current_azimuth = value();
        
        // Are we close? Then subtract from timer
        if (temp_current_azimuth == current_azimuth)
            timeout--;
        
        // Store current azimuth
        current_azimuth = temp_current_azimuth;
    }
    
    // Return success
    return true;
}

// Program a List of Azimuths
bool lars::azimuth::program_set (int step_time, int *points)
{
    /* TO-DO */

    // Return success
    return true;
}

// Get number of points in program
int lars::azimuth::program_points ()
{
    /* TO-DO */

    // Return success
    return true;
}

// Start Running the Program
bool lars::azimuth::program_start ()
{
    /* TO-DO */

    // Return success
    return true;
}

// Stop Running the Program
bool lars::azimuth::program_stop ()
{
    /* TO-DO */

    // Return success
    return true;
}

// Run Offset Calibration
bool lars::azimuth::calibrate_offset ()
{
    /* TO-DO */

    // Return success
    return true;
}

// Run Full Calibration
bool lars::azimuth::calibrate_full ()
{
    /* TO-DO */

    // Return success
    return true;
}