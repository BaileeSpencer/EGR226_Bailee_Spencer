#include "msp.h"

int flag_timer, flag_up, flag_down, count;  //global variables for interrupt functions

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
 * Brief: initialization of systick timer with reload value of 0.5s
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
 * Brief: Handler function for systick timer- increments count variable for button1 or decrements
 *      count variable for button2 and sets timer flag for main function
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_Handler(void)
{
    flag_timer = 1; // set flag for timeout of SysTick
    if (flag_up == 1){
        count++; // increment number on 7-segment LED display
        if (count == 10){
            count = 0;
        }
    }
    else if (flag_down == 1){
        if (count == 0){
            count = 9;
        }
        else {
            count--; // decrement number on 7-segment LED display
        }
    }
}

/****| Port5_init  | *****************************************
 * Brief: initialization of pins P5.0 and P5.1 to be used for buttons with port interrupts
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


    P5->IFG = 0; // Clear all P1 interrupt flags
}

/****| PORT5_IRQHandler  | *****************************************
 * Brief: Handler function for port interrupts- sets flag corresponding to either count up or down on the LED display depending
 *      on what button was pressed
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void PORT5_IRQHandler(void) // Port5 ISR
{
    if ( P5->IFG & BIT0 ){  // If P5.0 had an interrupt (going from high to low)
        flag_up = 1;
        flag_down = 0;
    }
    else if ( P5->IFG & BIT1 ){  // If P5.1 had an interrupt (going from high to low)
        flag_up = 0;
        flag_down = 1;
    }
    P5->IFG = 0; // Clear all P1 interrupt flags
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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    SysTick_Init();
    pin_init();
    Port5_init();
    NVIC_EnableIRQ(PORT5_IRQn);
    __enable_irq();
    count = 0;
    P4->OUT |= 0x3F;
    flag_up = 1;

    while(1){
        if(flag_timer == 1){
            count_display();
            flag_timer = 0;
        }
    }
}
