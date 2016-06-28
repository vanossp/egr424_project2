//*****************************************************************************
//
// timerdriver.h
//
// Author: Phillip VanOss
//		   Nick Shrock
//
// Purpose: This file serves to initialize and provide the functionality 
// 			required for a single timer on the ek-lm3s6965 TI board. This
// 			driver was written for use in the EGR 424 class for project 2.
//
//*****************************************************************************

#ifndef __TIMERDRIVER_H__
#define __TIMERDRIVER_H__

// Provide for C++ compilier
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Defines for proper operation
//
//*****************************************************************************
#define TWO_HUNDRED_MS		1000


//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void Timer_Init(void);
extern void Timer_IntRegister(void (*pfnHandler)(void));
extern void Timer_IntClear(void);


#ifdef __cplusplus
}
#endif

#endif // __TIMERDRIVER_H__
