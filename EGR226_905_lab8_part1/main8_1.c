#include "msp.h"

/****| pinint  | *****************************************
 * Brief: initialization of pins used for the motor
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pinint(){

    P2->SEL0 &= ~BIT4;
    P2->SEL1 &= ~BIT4;  //clear select registers, selecting gpio
    P2->DIR |= BIT4;  //configuring pin as outputs
    //P2->REN   ren is only for inputs
    P2->OUT &= ~BIT4;
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

	SysTick_Init();
	pinint();
	float duty_cycle = 0.5;
	float Ton = 0.0;
	float Toff = 0.0;
	while(1){
	    Ton = (duty_cycle * 25);
	    Toff = (25 - Ton);
	    P2->OUT |= BIT4;
	    Systick_ms_delay (Ton);
	    P2->OUT &= ~BIT4;
	    Systick_ms_delay (Toff);
	}

}
