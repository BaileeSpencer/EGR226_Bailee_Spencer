#include "msp.h"

int flag, count; //global variables for interrupt functions

/****| pin_init | *****************************************
 * Brief: This function initializes P4.0-P4.6 to use for the 7-segment LED display
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pin_init(){
    //7-segment LED display is using P4.0-P4.6, excludes decimal point for this lab
    P4->SEL0 &= ~0x7F;
    P4->SEL1 &= ~0x7F;  //clear select registers, selecting gpio
    P4->DIR |= 0x7F;  //configuring pins as outputs
    //P4->REN   ren is only for inputs
    P4->OUT &= ~0x7F;  //LEDs are initally off
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
 * Brief: Handler function for systick timer- increments count variable and sets flag for main function
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_Handler(void)
{
    flag = 1; // set flag for timeout of SysTick
    count++; // increment number on 7-segment LED display
    if (count == 10){
        count = 0;
    }
}

/****| count_display  | *****************************************
 * Brief: uses global variable count to determine what LED segments need to be turned on to display certain number
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void count_display (void){
    if (count == 0){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x3F;
    }
    else if (count == 1){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x06;
    }
    else if (count == 2){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x5B;
    }
    else if (count == 3){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x4F;
    }
    else if (count == 4){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x66;
    }
    else if (count == 5){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x6D;
    }
    else if (count == 6){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x7D;
    }
    else if (count == 7){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x07;
    }
    else if (count == 8){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x7F;
    }
    else if (count == 9){
        P4->OUT &= ~0x7F;
        P4->OUT |= 0x67;
    }
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    SysTick_Init();
    pin_init();
    __enable_irq();
    count = 0;
    P4->OUT |= 0x3F;

    while(1){
        if(flag == 1){
            count_display();
            flag = 0;
        }
    }

}
