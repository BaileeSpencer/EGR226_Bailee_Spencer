#include "msp.h"
#include <stdio.h>

/****| SysTick_Init  | *****************************************
 * Brief: initialization of systic timer
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_Init (void) {
    SysTick -> CTRL = 0; // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF; // max reload value
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}

/****| SysTick_delay  | *****************************************
 * Brief: Systick delay function
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void SysTick_delay (uint16_t delay) {
    SysTick -> LOAD = ((delay * 3000) - 1); //delay for 1 msecond per delay value
    SysTick -> VAL = 0; // any write to CVR clears it
    while ( (SysTick -> CTRL & 0x00010000) == 0); // wait for flag to be SET
}

/****| keypad_init  | *****************************************
 * Brief: This function initializes the rows and columns of the keypad
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void keypad_init(){
    //Rows of keypad are connected to pins P4.0, P4.1, P4.2, and P4.3
    P4->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3);  //clear select registers, selecting gpio
    P4->DIR &= ~(BIT0|BIT1|BIT2|BIT3);  //configuring pins as inputs
    P4->REN |= (BIT0|BIT1|BIT2|BIT3);  //pull up resistors
    P4->OUT |= (BIT0|BIT1|BIT2|BIT3);

    //Columns of keypad are connected to pins P4.4, P4.5, and P4.6
    P4->SEL0 &= ~(BIT4|BIT5|BIT6);
    P4->SEL1 &= ~(BIT4|BIT5|BIT6);  //clear select registers, selecting gpio
    P4->DIR &= ~(BIT4|BIT5|BIT6);  //configuring pins as inputs
    //P4->REN |= (BIT4|BIT5|BIT6);  //pull up resistors
    P4->OUT |= (BIT4|BIT5|BIT6);
}

/****| Read_Keypad | *****************************************
 * Brief: This function reads the number pressed on a keypad
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
uint8_t Read_Keypad(void) {
    uint8_t col, row, num;
    for ( col = 0; col < 3; col++ ) {
        P4->DIR = 0x00; // Set Columns to inputs
        P4->DIR |= BIT(4 + col); // Set column 3 to output
        P4->OUT &=~ BIT(4 + col); // Set column 3 to LOW
        SysTick_delay (10); // Delay the while loop
        row = P4->IN & 0x0F; // read all rows
        while ( !(P4IN & BIT0) | !(P4IN & BIT1) | !( P4IN & BIT2) | !( P4IN & BIT3) );
        if (row != 0x0F) break; // if one of the input is low, some key is pressed.
    }
    P4->DIR = 0x00; // Set Columns to inputs
    if ( col == 3)
        return 0;
    if (row == 0x0E) num = (col + 1); // key in row 0
    if (row == 0x0D) num = (3 + col +1); // key in row 1
    if (row == 0x0B) num = (6 + col +1); // key in row 2
    if (row == 0x07) num = (9 + col +1); // key in row 3
    return num;
}
/****| Print_Keys | *****************************************
 * Brief: This function prints the number pressed on a keypad
 * param:
 *      int val
 * return:
 *      n/a
 *************************************************************/

void Print_Keys (int val) {
    if (val == 10){
        printf("key [*]\n");
    }
    else if (val == 11){
        printf("key [0]\n");
    }
    else if (val == 12){
        printf("key [#]\n");
    }
    else {
        printf("key [%d]\n", val);
    }
}

void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    SysTick_Init ();
    keypad_init();

    printf("Enter any key\n");

    while (1) {
        int pressed = Read_Keypad ( ); // Call Function to read Keypad
        if (pressed)
            Print_Keys (pressed);
        __delay_cycles(30000); // 10ms delay through the loop before reading keypad again
    }
}
