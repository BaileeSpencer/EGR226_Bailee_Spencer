#include "msp.h"
// Changed code to be for servo motor now
void TimerA_init(void) {
    P5->SEL0 |= BIT6;
    P5->SEL1 &= ~(BIT6);
    P5->DIR |= BIT6; // P2.4 set TA0.1


    TIMER_A2->CCR[0] = 30000 - 1; // PWM Period (# cycles of clock)
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A2->CCR[1] = 0; // CCR1 PWM duty cycle in 10ths of percent
    TIMER_A2->CTL = 0x0254; // SMCLK, Up Mode, /2 divider, clear TAR to start
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    TimerA_init();



    while(1) {
        TIMER_A2->CCR[1] = 750;
        __delay_cycles((1500000));
        TIMER_A2->CCR[1] = 4000;
        __delay_cycles((3000000));
        }
    }

