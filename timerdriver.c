//*****************************************************************************
//
// timerdriver.c
//
// Authors: Phillip VanOss
//		    Nick Schrock
//
// Purpose: This file serves to initialize and provide the functionality 
// 			required for a single timer on the ek-lm3s6965 TI board. This
// 			driver was written for use in the EGR 424 class for project 2.
//
//*****************************************************************************

// Include Files
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

#include "timerdriver.h"


//*****************************************************************************
//
// Timer Init
//
// Initializes a 16KHz timer for use in the EGR 424 Sound Visualizer project
// using the EK-LM3S6965 Evaluation board.
//
//*****************************************************************************
void Timer_Init(void)
{
	// Enable the timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	// Configure a periodic timer
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	// Set the timer to trigger every 0.0625 ms = 16 KHz
	TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet()/16000));

	// Enable the timer interrupts
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Enable the timer
	TimerEnable(TIMER0_BASE, TIMER_A);
}


//*****************************************************************************
//
// Timer Interrupt Register
//
// Registers a desired function as the interrupt handler for a timer.
//
//*****************************************************************************
void Timer_IntRegister(void (*pfnHandler)(void))
{
	TimerIntRegister(TIMER0_BASE, TIMER_A, pfnHandler);
}


//*****************************************************************************
//
// Timer Interrupt Clear
//
// Clears the interrupt set by a timer.
//
//*****************************************************************************
void Timer_IntClear(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}