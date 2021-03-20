#include "msp.h"

void TimerA_init(void) {
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1


    TIMER_A0->CCR[0] = 37500 - 1; // PWM Period (# cycles of clock)
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A0->CCR[1] = (37500 * 0.7); // CCR1 PWM duty cycle in 10ths of percent
    TIMER_A0->CTL = 0x0254; // SMCLK, Up Mode, /2 divider, clear TAR to start
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    TimerA_init();
    float duty_cycle = 0.7;

    while(1) {
        TIMER_A0->CCR[1] = (37500 * duty_cycle);
    }

}
