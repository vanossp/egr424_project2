//*****************************************************************************
//
// adcdriver.c
//
// Authors: Phillip VanOss
//          Nick Schrock
//
// Purpose: This file serves to initialize and provide the functionality 
// 			required for a single ended interrupt driven adc input on a
//			specific pin of the ek-lm3s6965 TI board. This file was written to
//			fulfill the requirements of project 2 for EGR 424.
//
//*****************************************************************************

// Include Files
#include "inc/hw_adc.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

#include "adcdriver.h"


//*****************************************************************************
//
// ADC Init
//
// Initializes a 10 bit analog to digital converter to take
// a sample every 0.0625 ms and set an interrupt upon sampling completion.
//
//*****************************************************************************
void ADC_Init(void)
{
    // Set the clock to run on a 20 MHz frequency
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Enable the adc perphiral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable the gpio to allow for the adc
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Set up the correct pin as the adc
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);

    // Set up the ADC on ADC0 to read from sequence 3
    ADCSequenceDisable(ADC0_BASE, 3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);

    // Enable ADC interrupts
    ADCIntEnable(ADC0_BASE, 3);

    // Clear the ADC interrupt flag
    ADCIntClear(ADC0_BASE, 3);
}


//*****************************************************************************
//
// ADC Interrupt Clear
//
// Clears a interrupt that is set after a sample has completed.
//
//*****************************************************************************
void ADC_IntClear(void)
{
    ADCIntClear(ADC0_BASE, 3);
}


//*****************************************************************************
//
// ADC Interrupt Register
//
// Takes in a pointer to a function that sets up that function as the routine
// to run when the ADC sample complete interrupt is set.
//
//*****************************************************************************
void ADC_IntRegister(void (*pfnHandler)(void))
{
	ADCIntRegister(ADC0_BASE, 3, pfnHandler);
}


//*****************************************************************************
//
// ADC Start Sample
//
// Triggers an ADC Value to be read.
//
//*****************************************************************************
void ADC_StartSample(void)
{
    ADCProcessorTrigger(ADC0_BASE, 3);
}


//*****************************************************************************
//
// ADC Get Sample
//
// Returns the result of an ADC event.
//
//*****************************************************************************
void ADC_GetSample(unsigned long *value)
{
    ADCSequenceDataGet(ADC0_BASE, 3, value);
}