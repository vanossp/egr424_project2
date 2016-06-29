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
unsigned uTimerCount = 0;
unsigned long ulBuffer1[10], ulBuffer2[10];
unsigned uIndexCounter = 0;
unsigned uBufferSelect;
double dAvgADCVal = 0;
char cAdcADCVal;


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
			uTimerFlag = 0;

			uTimerCount++;

			if(uTimerCount == 100)
			{
				uTimerCount = 0;
				dAvgADCVal = 0; // reset average

				if(uBufferSelect == 1)
				{
					//RIT128x96x4Clear();
					RIT128x96x4StringDraw("Hello", 36,  0, 15);
					// switch buffers early so the adc doesnt overwrite anything or mess up
					uBufferSelect = 0;

					// average second buffer
					for(uIndexCounter=0; uIndexCounter<uTimerCount; uIndexCounter++)
					{
						dAvgADCVal += ulBuffer2[uIndexCounter]; // grab index and roll into average
						ulBuffer1[uIndexCounter] = 0; // reset array index
					}
				}
				else
				{
					//RIT128x96x4Clear();
					RIT128x96x4StringDraw("Bye", 36,  16, 15);
					// switch buffers
					uBufferSelect = 1;

					// average first buffer
					for(uIndexCounter=0; uIndexCounter<uTimerCount; uIndexCounter++)
					{
						dAvgADCVal += ulBuffer1[uIndexCounter]; // grab index and roll into average
						ulBuffer1[uIndexCounter] = 0; // reset array index
					}
				}
				// average and display
				dAvgADCVal /= 10;
				cAdcADCVal = (char)dAvgADCVal;
				//RIT128x96x4StringDraw(&cAdcADCVal, 2, 2, 15);
				//RIT128x96x4StringDraw("Hello", 36,  0, 15);
			}

			//RIT128x96x4StringDraw("UART Echo", 36,  0, 15);

			ADC_StartSample();
		}

		if(uAdcFlag == 1)
		{
			uAdcFlag = 0;

			//RIT128x96x4Clear();

			//fill buffer here
			if(uBufferSelect == 1)
			{
				ADC_GetSample(&ulBuffer1[uTimerCount-1]);
			}
			else
			{
				ADC_GetSample(&ulBuffer2[uTimerCount-1]);
			}
		}
	}
}