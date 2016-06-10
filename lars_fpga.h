/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lars_fpga.h
 * Author: David
 *
 * Created on March 30, 2016, 1:44 PM
 */

#ifndef LARS_FPGA_H
#define LARS_FPGA_H

// LARS Variables
#include "lars_variables.h"

namespace lars
{
    class fpga
    {
    public:

        // Enum - Trasmitter Directions
        enum tx_dir
        {
            TX_DIR_OFF = 0x00,
            TX_DIR_CALIBRATE = 0x01,
            TX_DIR_HORIZONTAL = 0x02,
            TX_DIR_VERTICAL = 0x03

        };

        // Enum - Switch  Register Bits
        enum sw_bits
        {
            SW_1 = 0,
            SW_2 = 1,
            SW_3 = 2
        };

        // Enum - PWM Enable Bits
        enum pwm_bits
        {
            PWM_SW_1 = 0,
            PWM_SW_2 = 1,
            PWM_SW_3 = 2,
            BLK_SET = 3
        };

        // Constructor
        fpga ( tty ttyDevice );

        // Destructor
        virtual ~fpga ( );

        // Update the assigned TTY
        void update_tty ( tty ttyDevice );

        // Check if the device was initialized
        bool is_initialized ( );

        // Read Functions
        float read_frequency ( ); // Returns the pulse repetition frequency
        float read_range ( ); // Returns the maximum range
        float read_duty ( ); // Returns the duty
        int read_tx_direction ( ); // Returns the output direction of tx
        bool read_rxA_switch ( ); // Returns the on/off state of chA (SW1)
        bool read_rxB_switch ( ); // Returns the on/off state of chB (SW3)
        bool read_tx_switch ( ); // Returns the on/off state of tx (SW2)
        bool read_pwm_enabled ( ); // Returns the on/off state of the PWM block
        bool read_pwm_chA_enabled ( ); // Returns if chA is using the PWM
        bool read_pwm_chB_enabled ( ); // Returns if chB is using the PWM
        bool read_pwm_tx_enabled ( ); // Returns if tx is using the PWM

        // Set Functions
        float set_frequency ( float desired_frequency, float desired_duty ); // Returns actual value set due to rounding
        float set_range ( float desired_range, float desired_duty ); // Returns actual value set due to rounding
        //bool set_duty ( float ); // Returns actual value set due to rounding
        bool set_tx_direction ( int ); // Sets the output direction of the tx line
        bool set_rxA_switch ( bool ); // Sets SW1 closed(1) or opened (0)
        bool set_rxB_switch ( bool ); // Sets SW3 closed(1) or opened (0)
        bool set_tx_switch ( bool ); // Sets SW2 closed(1) or opened (0)
        bool set_pwm_enabled ( bool ); // Sets if the PWM block is en(1) or dis(0)
        bool set_pwm_chA_enabled ( bool ); // Sets chA is using the PWM
        bool set_pwm_chB_enabled ( bool ); // Sets chB is using the PWM
        bool set_pwm_tx_enabled ( bool ); // Sets tx is using the PWM

    private:

        // Private Methods
        bool initialize ( ); // Initialize the FPGA board
        bool restart_pwm_block ( ); // Restarts the PWM block

        // Private Serial Interface
        TimeoutSerial serial; // Serial object used to interface
        std::string dev_name; // TTY device name
        std::string dev_address; // TTY device address
        std::string dev_serialnumber; // TTY device serial number
        int dev_baud; // TTY device baud

        // Private Variables
        float sys_clock; // The system clock
        bool initialized; // Was the device able to initialize?
    };

}

#endif /* LARS_FPGA_H */

