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
 * Brief: initialization of TimerA with PWM on p2.4 and capture interrupts on p2.7
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void TimerA0Init(void)
{
P2->SEL0 |= (BIT4|BIT7);
P2->SEL1 &=~ (BIT4|BIT7);
P2->DIR |= (BIT4|BIT7); // P2.4 set TA0.1, P2.7 set TA0.4
TIMER_A0->CTL = (TIMER_A_CTL_SSEL__SMCLK|TIMER_A_CTL_MC__UP|TIMER_A_CTL_CLR|TIMER_A_CTL_ID__8); //Divide clock by 8, SMCLK, Up mode, clear timer
TIMER_A0->CCR[0] = 37500; // 14 Hz
TIMER_A0->CCR[1] = (37500/2); // 50% duty cycle
TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;

TIMER_A0->CCTL[4] = 0x4114;  // capture mode with interrupts
}

/****| pininit | *****************************************
 * Brief: This function initializes the red on board LED on P2.0
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pininit(){
    //Red LED is on P2.0
    P2->SEL0 &= ~(BIT0);
    P2->SEL1 &= ~(BIT0);  //clear select registers, selecting gpio
    P2->DIR |= (BIT0);  //configuring pins as outputs
    //P2->REN   ren is only for inputs
    P2->OUT &= ~(BIT0);  //LED is initally off
}


void TA0_4_IRQHandler(void){
    TIMER_A0->CCTL[4] &=~ BIT0;
    P2->OUT ^= BIT0;
}


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    TimerA0Init();
    pininit();

    __enable_irq();
    while(1);

}
