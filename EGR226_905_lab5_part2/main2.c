#include "msp.h"

void SysTick_Init (void)
{ //initialization of systic timer
SysTick -> CTRL = 0; // disable SysTick During step
SysTick -> LOAD = 0x00FFFFFF; // max reload value
SysTick -> VAL = 0; // any write to current clears it
SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}

void SysTick_delay (int delay)
{ // Systick delay function
SysTick->LOAD = ((delay * 3000) - 1); //delay for 1 msecond per delay value
SysTick->VAL = 0; // any write to CVR clears it
while ( (SysTick->CTRL & 0x00010000) == 0); // wait for flag to be SET
}

/****| pinint | *****************************************
 * Brief: This function initializes red, green, and blue LED as well as the button connected to P1.4
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pinint(){
    //LED are using P2.4, P2.5, P2.6
    P2->SEL0 &= ~(BIT4|BIT5|BIT6);
    P2->SEL1 &= ~(BIT4|BIT5|BIT6);  //clear select registers, selecting gpio
    P2->DIR |= (BIT4|BIT5|BIT6);  //configuring pins as outputs
    //P2->REN   ren is only for inputs
    P2->OUT &= ~(BIT4|BIT5|BIT6);  //LEDs are initally off

    //button is on P3.7
    P3->SEL0 &= ~BIT7;
    P3->SEL1 &= ~BIT7;
    P3->DIR &= ~BIT7;  //configured button as input
    P3->REN |= BIT7;  //pull up resistor
    P3->OUT |= BIT7;
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
    if ((P3->IN & BIT7) == 0) //check if button pushed
    { SysTick_delay(10); //pause for 10 m-sec for switch bounce
    if ((P3->IN & BIT7) == 0) //check of button is still pushed
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
    if (P3->IN & BIT7) //check if button released
    { SysTick_delay(10); //pause for 10 m-sec for switch bounce
    if (P3->IN & BIT7) //check of button is still released
        pin_Value = 1;
    }
    return pin_Value; //return 1 if pushed- 0 if not pushed
}


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer


    SysTick_Init();
    pinint();


    while(1){
        if ((P3->IN & BIT7) == 0){
            if (DebounceSwitch1() ){
                while (1) {

                    if((P2->OUT & (BIT4|BIT5|BIT6)) == 0){     //turns on green LED when all LEDs are off
                        P2->OUT |= BIT4;
                    }
                    else if(P2->OUT & BIT4){                //turns off green LED and turns on yellow LED when button pressed
                        P2->OUT &= ~BIT4;
                        P2->OUT |= BIT5;
                    }
                    else if(P2->OUT & BIT5){                //turns off yellow LED and turns on red LED when button pressed
                        P2->OUT &= ~BIT5;
                        P2->OUT |= BIT6;
                    }
                    else if(P2->OUT & BIT6){                //turns off red LED and turns on green LED when button pressed
                        P2->OUT &= ~BIT6;
                        P2->OUT |= BIT4;
                    }

                    if(P3->IN & BIT7){                     //checks for button released
                        if( DebounceSwitchOff()) {
                            break;
                        }
                    }

                    SysTick_delay(1000);

                    if(P3->IN & BIT7){                     //checks for button released
                        if( DebounceSwitchOff()) {
                            break;
                        }
                    }
                }
            }
        }
    }
}



