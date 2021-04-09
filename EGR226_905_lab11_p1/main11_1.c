/**************************************************************************************
 * Author:      Bailee Spencer
 * Course:      EGR 226 - 905
 * Date:        4/9/2021
 * Project:     Precise Timing Intervals using Infrared (IR) Signals
 * File:        TitleBlock.c
 * Description: This program generates and detects IR signals.
 * **************************************************************************************/
#include "msp.h"

/****| TimerA0Init  | *****************************************
 * Brief: initialization of TimerA with PWM on p2.4
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void TimerA0Init(void)
{
P2->SEL0 |= BIT4;
P2->SEL1 &=~ BIT4;
P2->DIR |= BIT4; // P2.4 set TA0.1
TIMER_A0->CTL = (TIMER_A_CTL_SSEL__SMCLK|TIMER_A_CTL_MC__UP|TIMER_A_CTL_CLR|TIMER_A_CTL_ID__8); //Divide clock by 8, SMCLK, Up mode, clear timer
TIMER_A0->CCR[0] = 37500; // 14 Hz
TIMER_A0->CCR[1] = (37500/2); // 50% duty cycle
TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
}


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	TimerA0Init();
	while(1);

}
