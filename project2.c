//*****************************************************************************
//
// project2.c
//
// Authors: Phillip VanOss
//			Nick Schrock
//
// Purpose: This file serves as the main file for the audio visualizer for 
//  		EGR 424.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "drivers/rit128x96x4.h"

#include "adcdriver.h"
#include "timerdriver.h"

// Define the number of timer counts to reach two hundred ms, use for array sizes too
#define TWO_HUNDRED_MS 3200

// Declare interrupt variables
volatile unsigned uAdcFlag;
volatile unsigned uTimerFlag;

// Declare buffer variables
unsigned long ulBuffer1[TWO_HUNDRED_MS], ulBuffer2[TWO_HUNDRED_MS];
unsigned uBufferSelect;


//*****************************************************************************
//
// ADC Interrupt Handler
//
// Triggers when an ADC event has finished reading. Clears the interrupt and
// sets a flag.
//
//*****************************************************************************
void ADC_IsrHandler()
{
	ADC_IntClear(); // clear interrupt
	uAdcFlag = 1; // raise flag
}


//*****************************************************************************
//
// Timer Interrupt Handler
//
// Triggered by a timer after a set amount of time. Clears the interrupt,
// starts an ADC sample, and raises a flag.
//
//*****************************************************************************
void Timer_IsrHandler()
{
	Timer_IntClear(); // clear interrupt
	ADC_StartSample(); // start adc sample
	uTimerFlag = 1; // raise flag
}


//*****************************************************************************
//
// Average Buffer
//
// Takes a buffer of a certain length and computes the average of the buffer.
//
//*****************************************************************************
double Average_Buffer(unsigned long* buffer)
{
	double average = 0;
	double length = TWO_HUNDRED_MS; // length of the buffer to be averaged

	// sum the buffer into one variable 
	for(int i=0;i<length;i++)
	{
		average += buffer[i];
	}

	// determine the average by dividing the summed buffer by its length
	average /= length;

	return average;
}


//*****************************************************************************
//
// OLED Display Power Level
//
// Takes in a value indicative of the power level on an adc pin and displays it
// as a bar that changes height and brightness according to the power level
//
//*****************************************************************************
void OLED_DisplayPowerLvl(double power)
{
	double amplification = 20; // software amplifier

	unsigned intensity, index;

	unsigned Y_LIM = 96; // max height of the bar
	unsigned X_LIM = 10; // width of the bar

	unsigned char array[Y_LIM*X_LIM]; // array containing power bar information

	// Determine the intensity of the pixels in the power bar
	intensity = (int)(((power*amplification)/1023.0)*X_LIM);
	intensity = (int)(intensity*0.1579);

	// Shift over the intensity and or it back in to make full byte
	intensity = ((intensity << 4) | intensity);

	// Determine pixel specific information
	for(index=0;index < (X_LIM*Y_LIM/2);index++)
	{
		// Turn the power bar pixels to the correct intensity
		array[index] = intensity;		
	}

	// Draw the image stored in array
	RIT128x96x4ImageDraw(array, 0, 0, X_LIM, Y_LIM);
}


//*****************************************************************************
//
// Main
//
// Initializes all components and peripherals.
//
//*****************************************************************************
int main(void)
{
	// Declare variables used in main
	unsigned uTimerCount = 0;
	unsigned long ulAdcVal;
	double dAvgAdcVal = 0;

	// Initialize ADC
	ADC_Init();
	ADC_IntRegister(ADC_IsrHandler);

	// Initialize Timer
	Timer_Init();
	Timer_IntRegister(Timer_IsrHandler);

	// Initialize Screen
	RIT128x96x4Init(1000000);

	// Set up and prepare to enable ADC and timer interrupts
	IntEnable(INT_ADC0SS3);
	IntEnable(INT_TIMER0A);

	// Enable interruptes
	IntMasterEnable();

	while(1)
	{
		// only occurs after timer event
		if(uTimerFlag == 1)
		{
			// lower flag
			uTimerFlag = 0; 

			// update counter
			uTimerCount++; 

			// check if 200 ms have passed
			if(uTimerCount == TWO_HUNDRED_MS - 1)
			{
				// reset counter
				uTimerCount = 0; 

				// determine the average voltage level from the correct buffer
				dAvgAdcVal = (uBufferSelect == 1) ? Average_Buffer(ulBuffer1) : Average_Buffer(ulBuffer2);
				
				// switch buffers
				uBufferSelect ^= 1; 

				// Display the power level by adjusting a bar
				OLED_DisplayPowerLvl(dAvgAdcVal); 
			}
		}

		// only occurs after adc event
		if(uAdcFlag == 1)
		{
			// lower flag
			uAdcFlag = 0;

			// get an adc read
			ADC_GetSample(&ulAdcVal); 

			//check which buffer is active and fill the appropriate one
			if(uBufferSelect == 1)
				ulBuffer1[uTimerCount] = ulAdcVal;
			else
				ulBuffer2[uTimerCount] = ulAdcVal;
		}
	}
}