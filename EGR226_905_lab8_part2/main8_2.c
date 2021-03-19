#include "msp.h"


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	P2->SEL0 |= BIT4;
	P2->SEL1 &= ~(BIT4);
	P2->DIR |= BIT4; // P2.4 set TA0.1

	int duty_cycle = 500;

	TIMER_A0->CCR[0] = 1000 - 1; // PWM Period (# cycles of clock)
	TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
	TIMER_A0->CCR[1] = duty_cycle; // CCR1 PWM duty cycle in 10ths of percent
	TIMER_A0->CTL = 0x0214;

}
