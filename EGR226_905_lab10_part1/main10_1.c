#include "msp.h"
#include <stdio.h>

/****| adcsetup  | *****************************************
 * Brief: initialization of ADC on p5.5 starting in memory location 5
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void adcsetup(void)
{
    ADC14->CTL0 = 0x00000010; //power on and disabled during configuration
    ADC14->CTL0 |= 0x04D80300; // S/H pulse mode, MCLCK, 32 sample clocks, sw trigger, /4 clock divide
    ADC14->CTL1 = 0x00000030; // 14-bit resolution
    ADC14->MCTL[5] = 0; // A0 input, single ended, vref=avcc
    P5->SEL1 |= 0x20; // configure pin 5.5 for AO
    P5->SEL0 |= 0x20;
    ADC14->CTL1 |= 0x00050000; //start converting at mem reg 5
    ADC14->CTL0 |= 2; //enable ADC after configuration
}

/****| SysTick_Init  | *****************************************
 * Brief: initialization of systic timer
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_Init (void) {
    SysTick -> CTRL = 0; // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF; // max reload value
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}
/****| Systick_ms_delay  | *****************************************
 * Brief: Systick delay function using ms
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void Systick_ms_delay (uint16_t delay) {
    if (delay == 0){
        return;
    }
    SysTick -> LOAD = ((delay * 3000) - 1); //delay for 1 msecond per delay value
    SysTick -> VAL = 0; // any write to CVR clears it
    while ( (SysTick -> CTRL & 0x00010000) == 0); // wait for flag to be SET
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    adcsetup();
    SysTick_Init();
    int result;
    float voltage;

    while (1){
        ADC14->CTL0 |=1; //start a conversion
        while(!ADC14->IFGR0); // wait until conversion complete
        result = ADC14->MEM[5]; // immediately store value in a variable
        voltage = (result*0.000201);
        printf ("Raw ADC value is: %d\n", result);
        printf ("Voltage is: %f\n", voltage);
        Systick_ms_delay(500); // wait for next value- using Systick function
    }

}
