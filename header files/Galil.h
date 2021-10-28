/*****************
Galil.h v5
JAMES STEVENS 2020
*****************/

/*
This object class outlines all the required methods for mtrn3500 Assignment 1
The functions declared in this file should be completed by the student in a file of their own making called "Galil.cpp"
DO NOT call Galil functions directly. They must all be called through EmbeddedFunctions, for example
EmbeddedFunctions *F, F->GCcommand.
Read the comments for each funciton for more information on the required implementation.
You are allowed to add extra members, data structures and functions, but you must not modify the existing members.
Galil commands are sent as ASCII. All commands will be sent to the board as a string of characters, and must be
terminated by a semicolon ';'
*/

#pragma once
#include <stdint.h>
#include<stdio.h>
#include "EmbeddedFunctions.h"
#include <string>
#include <iostream>
#include <chrono>
#include <Windows.h>

class Galil {
	public:
		Galil();												// Default constructor. Initialize variables, open Galil connection and allocate memory. NOT AUTOMARKED
		Galil(EmbeddedFunctions* Funcs, GCStringIn address);	// Constructor with EmbeddedFunciton initialization
		~Galil();												// Default destructor. Deallocate memory and close Galil connection. NOT AUTOMARKED

		// DIGITAL OUTPUTS
		void DigitalOutput(uint16_t value);						// Write to all 16 bits of digital output, 1 command to the Galil
		void DigitalByteOutput(bool bank, uint8_t value);		// Write to one byte, either high or low byte, as specified by user in 'bank'
																// 0 = low, 1 = high
		void DigitalBitOutput(bool val, uint8_t bit);			// Write single bit to digital outputs. 'bit' specifies which bit


		// DIGITAL INPUTS
		uint16_t DigitalInput();				// Return the 16 bits of input data
												// Query the digital inputs of the GALIL, See Galil command library @IN
		uint8_t DigitalByteInput(bool bank);	// Read either high or low byte, as specified by user in 'bank'
												// 0 = low, 1 = high
		bool DigitalBitInput(uint8_t bit);		// Read single bit from current digital inputs. Above functions
												// may use this function

		bool CheckSuccessfulWrite();	// Check the string response from the Galil to check that the last 
										// command executed correctly. 1 = succesful. NOT AUTOMARKED

		// ANALOG FUNCITONS
		float AnalogInput(uint8_t channel);						// Read Analog channel and return voltage			
		void AnalogOutput(uint8_t channel, double voltage);		// Write to any channel of the Galil, send voltages as
																// 2 decimal place in the command string
		void AnalogInputRange(uint8_t channel, uint8_t range);	// Configure the range of the input channel with
																// the desired range code

		// ENCODER / CONTROL FUNCTIONS
		void WriteEncoder();				// Manually Set the encoder value to zero
		int ReadEncoder();					// Read from Encoder
		void setSetPoint(int s);			// Set the desired setpoint for control loops, counts or counts/sec
		void setKp(double gain);			// Set the proportional gain of the controller used in controlLoop()
		void setKi(double gain);			// Set the integral gain of the controller used in controlLoop()
		void setKd(double gain);			// Set the derivative gain of the controller used in controlLoop()
		void PositionControl(bool debug);	// Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.
											// The loop will run using the PID values specified in the data of this object, and has an 
											// automatic timeout of 10s. You do not need to implement this function, it is defined in
											// GalilControl.lib. debug = 1 for debugging (it will print some values).
		void SpeedControl(bool debug);		// same as above. Setpoint interpreted as counts per second

		// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
		// output of GInfo and GVersion, with two newLines after each.
		friend std::ostream& operator<<(std::ostream& output, Galil& galil);


	protected:
		EmbeddedFunctions * Functions;	// Pointer to EmbeddedFunctions, through which all Galil Function calls will be made
		GCon g;							// Connection handle for the Galil, passed through most Galil function calls
		char ReadBuffer[1024];			// Buffer to restore responses from the Galil
		double ControlParameters[3];	// Contains the controller gain values: K_p, K_i, K_d in that order 
		int setPoint;					// Control Setpoint
};
