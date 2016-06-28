//*****************************************************************************
//
// adcdriver.c
//
// Author: Phillip VanOss
//         Nick Shrock
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
// ADC_Init
//
// Initializes a 10 bit analog to digital converter on pin _ in port _ to take
// a sample every __ ms and set an interrupt upon sampling completion.
//
//*****************************************************************************
void ADC_Init(void)
{
    //
    // Set the clocking to run at 20 MHz (200 MHz / 10) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock
    // source.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //
    // The ADC0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // For this example ADC0 is used with AIN0 on port E7.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.  GPIO port E needs to be enabled
    // so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    //
    // Select the analog ADC function for these pins.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);

    ADCSequenceDisable(ADC0_BASE, 3);

    //
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.  This example is arbitrarily using sequence 3.
    //
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // information on the ADC sequences and steps, reference the datasheet.
    //
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 3);

    ADCIntEnable(ADC0_BASE, 3);

    //
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
//
// ADC_IntClear
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
// ADC_IntRegister
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
// ADC_StartSample
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
// ADC_GetSample
//
// Returns the result of an ADC event
//
//*****************************************************************************
void ADC_GetSample(unsigned long *value)
{
    ADCSequenceDataGet(ADC0_BASE, 3, value);
}