#include "msp.h"
#include <stdio.h>

int flag;

/****| adcsetup  | *****************************************
 * Brief: initialization of ADC on p5.5 starting in memory location 5
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void adcsetup(void) {
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
 * Brief: initialization of systick timer
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_Init (void) {
    SysTick -> CTRL = 0; // disable SysTick During step
    SysTick -> LOAD = 1500000; // reload value for 0.5s interrupts
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000007; // enable systick, 3MHz, Interrupts
}
/****| SysTick_Handler  | *****************************************
 * Brief: Handler function for systick timer- sets flag for main function
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_Handler(void)
{
    flag = 1; // set flag for timeout of SysTick
}
/****| convertTemp  | *****************************************
 * Brief: uses ADC converter to print out temperature
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void convertTemp (void){
    int result;
    float voltage, celsius, Fahr;
    while (1){
        ADC14->CTL0 |=1; //start a conversion
        while(!ADC14->IFGR0); // wait until conversion complete
        result = ADC14->MEM[5]; // immediately store value in a variable
        voltage = (result*0.000201);
        celsius = ((voltage * 1000) - 500) / 10;
        Fahr = (celsius * 1.8) + 32;
        printf ("Temperature in °C: %f\n", celsius);
        printf ("Temperature in °F: %f\n", Fahr);
    }
}
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    adcsetup();
    SysTick_Init();

    while(1){
        if(flag == 1){
            convertTemp();
            flag = 0;
        }
    }
}
