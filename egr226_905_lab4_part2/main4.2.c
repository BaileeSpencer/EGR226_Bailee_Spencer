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
    P1->DIR &= ~BIT4;
    P1->REN |= BIT4;
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
    { __delay_cycles(30000); //pause for 10 m-sec for switch bounce
    if ((P1->IN & BIT4) == 0) //check of button is still pushed
        pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}

/****| DebounceSwitchOff | *****************************************
* Brief: This function is used for debouncing a switch. It checks for a button to be released, waits and then checks again
* to ensure it has been released.
* param:
*      n/a
* return:
*      n/a
*************************************************************/
int DebounceSwitchOff(void)
{
    int pin_Value = 0; //initialize variable as low
    if (P1->IN & BIT4) //check if button released
    { __delay_cycles(30000); //pause for 10 m-sec for switch bounce
    if (P1->IN & BIT4) //check of button is still released
        pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    pinint();

    while(1){
        if ((P1->IN & BIT4) == 0){
            if (DebounceSwitch1() ){
                while (1) {
                    P2->OUT &= ~BIT2;
                    P2->OUT |= BIT0; // turn on P2.1 red LED
                    if(P1->IN & BIT4){                     //checks for button released
                        if( DebounceSwitchOff()) {
                            break;
                        }
                    }
                    __delay_cycles(1000000);
                    P2->OUT &= ~BIT0;
                    P2->OUT |= BIT1; // turn on P2.1 green LED
                    if(P1->IN & BIT4){                     //checks for button released
                        if( DebounceSwitchOff()) {
                            break;
                        }
                    }
                    __delay_cycles(1000000);
                    P2->OUT &= ~BIT1;
                    P2->OUT |= BIT2; // turn on P2.1 blue LED
                    if(P1->IN & BIT4){                     //checks for button released
                        if( DebounceSwitchOff()) {
                            break;
                        }
                    }
                    __delay_cycles(1000000);

                }
            }
        }
    }

}



