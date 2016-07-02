//*****************************************************************************
//
// adcdriver.h
//
// Authors: Phillip VanOss
//		    Nick Schrock
//
// Purpose: This file serves to initialize and provide the functionality 
// 			required for a single ended interrupt driven adc input on a
//			specific pin of the ek-lm3s6965 TI board. This file was written to
//			fulfill the requirements of project 2 for EGR 424.
//
//*****************************************************************************

#ifndef __ADCDRIVER_H__
#define __ADCDRIVER_H__

// Provide for C++ compilier
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void ADC_Init(void);
extern void ADC_IntClear(void);
extern void ADC_IntRegister(void (*pfnHandler)(void));
extern void ADC_StartSample(void);
extern void ADC_GetSample(unsigned long *value);


#ifdef __cplusplus
}
#endif

#endif // __ADCDRIVER_H__
