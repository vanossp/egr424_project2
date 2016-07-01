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
unsigned long ulBuffer1[100], ulBuffer2[100];
unsigned uIndexCounter = 0;
unsigned uBufferSelect;
double dAvgADCVal = 0;
char cAdcADCVal, cTimerCount, cBufferSelect, cVal, str[100], test[100];


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

/*
void swap(char first, char second)
{
	char temporary;

	temporary = first;
	first = second;
	second = first;
}


// reverses a string
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}
 
// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    //bool isNegative = false;
    int isNegative = 0;
 
    // Handle 0 explicitely, otherwise empty string is printed for 0
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        //isNegative = true;
        isNegative = 1;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}*/


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
	double length = 100.0; // length of the buffer to be averaged

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
	unsigned height, intensity, index;
	double amplification = 20; // software amplifier
	unsigned char array[500]; // array containing power bar information
	unsigned Y_LIM = 40; // max height of the bar
	unsigned X_LIM = 10; // width of the bar

	//IntMasterDisable();

	height = (int)(((power*amplification)/1023.0)*X_LIM);

	intensity = (int)(height*0.1579);

	intensity = ((intensity << 4) | intensity);

	for(index=0;index < (X_LIM*Y_LIM/2);index++)
	{
		if(index < ((Y_LIM - height) * (X_LIM / 2.0) - 1))
		{
			array[index] = intensity;
		}
		else
		{
			array[index] = 0;
		}
	}

	RIT128x96x4ImageDraw(array, 0, 0, X_LIM, Y_LIM);

	//IntMasterEnable();
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
			if(uTimerCount == 99)
			{
				// reset counter
				uTimerCount = 0; 

				// determine the average voltage level from the correct buffer
				dAvgADCVal = (uBufferSelect == 1) ? Average_Buffer(ulBuffer1) : Average_Buffer(ulBuffer2);
				
				// switch buffers
				uBufferSelect ^= 1; 
				
				//itoa(dAvgADCVal*20.0, test, 10);

				// Display the power level by adjusting a bar
				OLED_DisplayPowerLvl(dAvgADCVal); 
			}
		}

		// only occurs after adc event
		if(uAdcFlag == 1)
		{
			// lower flag
			uAdcFlag = 0;

			// get an adc read
			ADC_GetSample(&ulAdcVal); 

			//itoa(ulAdcVal, str, 10);

			//check which buffer is active and fill the appropriate one
			if(uBufferSelect == 1)
				ulBuffer1[uTimerCount] = ulAdcVal;
			else
				ulBuffer2[uTimerCount] = ulAdcVal;
		}
	}
}