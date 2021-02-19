#include "msp.h"

/****| pinint | *****************************************
 * Brief: This function initializes red, green, and blue LED as well as the button connected to P1.4
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pinint(){
    //Rgb LED are P2.0, P2.1, P2.2
    P2->SEL0 &= ~(BIT0|BIT1|BIT2);
    P2->SEL1 &= ~(BIT0|BIT1|BIT2);  //clear select registers, selecting gpio
    P2->DIR |= (BIT0|BIT1|BIT2);  //configuring pins as outputs
    //P2->REN   ren is only for inputs
    P2->OUT &= ~(BIT0|BIT1|BIT2);  //LEDs are initally off

    //button is on P1.4
    P1->SEL0 &= ~BIT4;
    P1->SEL1 &= ~BIT4;
    P1->DIR &= ~BIT4;  //configured button as input
    P1->REN |= BIT4;  //pull up resistor
    P1->OUT |= BIT4;
}

/****| DebounceSwitch1 | *****************************************
 * Brief: This function is used for debouncing a switch. It checks for a button to be pressed, waits and then checks again
 * to ensure it is only pressed once
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
int DebounceSwitch1(void)
{
    int pin_Value = 0; //initialize variable as low
    if ((P1->IN & BIT4) == 0) //check if button pushed
    { __delay_cycles(10000); //pause for 10 m-sec for switch bounce
    if ((P1->IN & BIT4) == 0) //check of button is still pushed
        pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceSwitch_Hold | *****************************************
 * Brief: This function is used for debouncing a switch when it is held. It checks for a button to be pressed, waits and then checks again
 * to see if the button is still pressed. If it is still being held it waits again.
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
int DebounceSwitch_Hold(void)
{
    int pin_Value2 = 0; //initialize variable as low
    if ((P1->IN & BIT4) == 0) //check if button pushed
    { __delay_cycles(10000); //pause for 10 m-sec for switch bounce
    if ((P1->IN & BIT4) == 0) //check of button is still pushed
        pin_Value2 = 1;
    }
    return pin_Value2; //return 1 if held down- 0 if not pushed
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer\

    pinint();
    while(1){
        if((P1->IN & BIT4) == 0){                     //turns on red LED when button pressed and makes sure blue LED is off
            if( DebounceSwitch1()) {
                P2->OUT &= ~BIT2;
                P2->OUT &= ~BIT1;
                P2->OUT |= BIT0;
                while(DebounceSwitch_Hold()){                     //checks for button held
                    __delay_cycles(10000);
                }
            }
        }
        if((P1->IN & BIT4) == 0){                      //turns off red LED and turns on green LED when button pressed
            if( DebounceSwitch1()) {
                P2->OUT &= ~BIT0;
                P2->OUT &= ~BIT2;
                P2->OUT |= BIT1;
                while(DebounceSwitch_Hold()){                     //checks for button held
                    __delay_cycles(10000);
                }
            }
        }
        if((P1->IN & BIT4) == 0){                      //turns off green LED and turns on blue LED when button pressed
            if( DebounceSwitch1()) {
                P2->OUT &= ~BIT1;
                P2->OUT &= ~BIT0;
                P2->OUT |= BIT2;
                while(DebounceSwitch_Hold()){                     //checks for button held
                    __delay_cycles(10000);

                }
            }
        }
    }
}
