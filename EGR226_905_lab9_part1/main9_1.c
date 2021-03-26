#include "msp.h"

float duty_cycle = 0.7;  //global variable

/****| TimerA_init  | *****************************************
 * Brief: initialization of TimerA using PWM pin P2.4
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void TimerA_init(void) {
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1


    TIMER_A0->CCR[0] = 37500 - 1; // PWM Period (# cycles of clock)
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A0->CCR[1] = (37500 * 0.7); // CCR1 PWM duty cycle set to 70% initially
    TIMER_A0->CTL = 0x0254; // SMCLK, Up Mode, /2 divider, clear TAR to start
}

/****| Port5_init  | *****************************************
 * Brief: initialization of pins P5.0, P5.1, and P5.2 to be used for buttons with port interrupts
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void Port5_init (void){
    P5->SEL0 &=~ BIT0;
    P5->SEL1 &=~ BIT0;
    P5->DIR &=~ BIT0;
    P5->REN |= BIT0;  //pull up resistor
    P5->OUT |= BIT0; // Enable pull-up resistor
    P5->IES |= BIT0; //Set pin interrupt to trigger when it goes from high to low
    P5->IE |= BIT0; // Enable interrupt for P5.0

    P5->SEL0 &=~ BIT1;
    P5->SEL1 &=~ BIT1;
    P5->DIR &=~ BIT1;
    P5->REN |= BIT1;  //pull up resistor
    P5->OUT |= BIT1; // Enable pull-up resistor
    P5->IES |= BIT1; //Set pin interrupt to trigger when it goes from high to low
    P5->IE |= BIT1; // Enable interrupt for P5.1

    P5->SEL0 &=~ BIT2;
    P5->SEL1 &=~ BIT2;
    P5->DIR &=~ BIT2;
    P5->REN |= BIT2;  //pull up resistor
    P5->OUT |= BIT2; // Enable pull-up resistor
    P5->IES |= BIT2; //Set pin interrupt to trigger when it goes from high to low
    P5->IE |= BIT2; // Enable interrupt for P5.2

    P5->IFG = 0; // Clear all P1 interrupt flags
}

/****| PORT5_IRQHandler  | *****************************************
 * Brief: Handler function for port interrupts- increases, decreases, or stops motor according to what button is pressed
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void PORT5_IRQHandler(void) // Port5 ISR
{
    if ( P5->IFG & BIT0 ){  // If P5.0 had an interrupt (going from high to low)
        if(duty_cycle == 10){
            TIMER_A0->CCR[1] = (37500 * duty_cycle);
        }
        else {
            duty_cycle = duty_cycle + .1;
            TIMER_A0->CCR[1] = (37500 * duty_cycle);
        }

    }
    else if ( P5->IFG & BIT1 ){  // If P5.1 had an interrupt (going from high to low)
        if(duty_cycle == 0){
            TIMER_A0->CCR[1] = (37500 * duty_cycle);
        }
        else {
            duty_cycle = duty_cycle - .1;
            TIMER_A0->CCR[1] = (37500 * duty_cycle);
        }

    }
    else if ( P5->IFG & BIT2 ){  // If P5.2 had an interrupt
        duty_cycle = 0;
        TIMER_A0->CCR[1] = (37500 * duty_cycle);

    }
    P5->IFG = 0; // Clear all P1 interrupt flags
}


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer


    TimerA_init();
    Port5_init();
    NVIC_EnableIRQ(PORT5_IRQn);
    __enable_irq ();


    while(1) {
        ;
    }

}
