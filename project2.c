//*****************************************************************************
//
// project2.c
//
// Authors: Phillip VanOss
//			Nick Shrock
//
// Purpose: This file serves as the main file for the audio visualizer for 
//  		EGR 424.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "drivers/rit128x96x4.h"

#include "adcdriver.h"
#include "timerdriver.h"

volatile unsigned uAdcFlag;
volatile unsigned uTimerFlag;

unsigned long ulAdcVal;
unsigned uTimerCount;


void ADC_IsrHandler()
{
	ADC_IntClear();
	uAdcFlag = 1;

}


void Timer_IsrHandler()
{
	Timer_IntClear();
	uTimerFlag = 1;
}


int main(void)
{
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
		// If timer interrupt was triggered count up
		if(uTimerFlag == 1)
		{
			uTimerCount++;

			if(uTimerCount == TWO_HUNDRED_MS)
			{
				// average a buffer

				// display a buffer

				// clear a buffer

				// switch buffers
			}

			uTimerFlag = 0;

			ADC_StartSample();

			RIT128x96x4Clear();
		}

		if(uAdcFlag == 1)
		{
			uAdcFlag = 0;

			ADC_GetSample(&ulAdcVal);

			//fill buffer here

			//ADC_StartSample();
			

			//RIT128x96x4StringDraw("Hello World!", 30, 24, 15);
		}


	}
}