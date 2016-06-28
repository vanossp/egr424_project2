//*****************************************************************************
//
// timerdriver.c
//
// Author: Phillip VanOss
//		   Nick Shrock
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
// Timer_Init
//
//
//
//*****************************************************************************
void Timer_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet()/16000) -1);

	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer_IntRegister(void (*pfnHandler)(void))
{
	TimerIntRegister(TIMER0_BASE, TIMER_A, pfnHandler);
}

void Timer_IntClear(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}