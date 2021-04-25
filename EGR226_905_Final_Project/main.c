/**************************************************************************************
 * Author:      Bailee Spencer
 * Course:      EGR 226 - 905
 * Date:        4/23/2021
 * Project:     Final Project
 * File:        TitleBlock.c
 * Description: This program has a main menu where the user can choose to open a door,
 *                  change a motor speed, or change LED brightness
 * **************************************************************************************/
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "LCD_Library.h"
#include "LCD_CustomImages.h"
#include "SysTick_Library.h"

#define Main 0            //define all states 0-9
#define Door_menu 1
#define Door_open 2
#define Door_closed 3
#define Motor_menu 4
#define Motor 5
#define Lights_menu 6
#define Red 7
#define Green 8
#define Blue 9

int flag = 0;  //global variables
float red_duty, green_duty, blue_duty;
int key_flag = 0;

/****| keypad_init | *****************************************
 * Brief: This function initializes the keypad using pins p4.0-6
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
        __delay_cycles(30000); // Delay the while loop
        row = P4->IN & 0x0F; // read all rows
        while ( !(P4IN & BIT0) | !(P4IN & BIT1) | !( P4IN & BIT2) | !( P4IN & BIT3) );
        if (row != 0x0F) break; // if one of the input is low, some key is pressed.
    }
    P4->DIR = 0x00; // Set Columns to inputs
    if ( col == 3){
        key_flag = 0;
        return NULL;
    }
    if (row == 0x0E) num = (col + 1); // key in row 0
    if (row == 0x0D) num = (3 + col +1); // key in row 1
    if (row == 0x0B) num = (6 + col +1); // key in row 2
    if (row == 0x07) num = (9 + col +1); // key in row 3
    if (num == 11) num = 0;
    key_flag = 1;
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
    else if (val == 12){
        printf("key [#]\n");
    }
    else {
        printf("key [%d]\n", val);
    }
}
/****| TimerA_init  | *****************************************
 * Brief: initialization of TimerA0 using PWM pin P2.4 for DC motor,
 *          as well as TimerA0 on pins P2.5-P2.7 for the RGB LED PWM,
 *          and TimerA2 on pin P5.6 for the Servo motor
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void TimerA_init(void) {
    P2->SEL0 |= BIT4;               //TimerA0 on P2.4 for DC motor
    P2->SEL1 &= ~(BIT4);
    P2->DIR |= BIT4; // P2.4 set TA0.1

    TIMER_A0->CCR[0] = 37500 - 1; // PWM Period (# cycles of clock)         40Hz frequency
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A0->CCR[1] = (37500 * 0); // CCR1 PWM duty cycle in 10ths of percent
    TIMER_A0->CTL = 0x0254; // SMCLK, Up Mode, /2 divider, clear TAR to start

    P2->SEL0 |= BIT5;               //TimerA0 on P2.5 for Red LED
    P2->SEL1 &= ~(BIT5);
    P2->DIR |= BIT5; // P2.5 set TA0.2
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set mode 7
    TIMER_A0->CCR[2] = (37500 * 0); // CCR2 PWM duty cycle in 10ths of percent

    P2->SEL0 |= BIT6;               //TimerA0 on P2.6 for Green LED
    P2->SEL1 &= ~(BIT6);
    P2->DIR |= BIT6; // P2.6 set TA0.3
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set mode 7
    TIMER_A0->CCR[3] = (37500 * 0); // CCR2 PWM duty cycle in 10ths of percent

    P5->SEL0 |= BIT7;               //TimerA2 on P5.7 for Blue LED
    P5->SEL1 &= ~(BIT7);
    P5->DIR |= BIT7; // P2.7 set TA0.4
    TIMER_A2->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set mode 7
    TIMER_A2->CCR[2] = (37500 * 0); // CCR2 PWM duty cycle in 10ths of percent

    P5->SEL0 |= BIT6;               //TimerA2 on P5.6 for Servo motor
    P5->SEL1 &= ~(BIT6);
    P5->DIR |= BIT6; // P5.6 set TA2.1

    TIMER_A2->CCR[0] = 30000 - 1; // PWM Period (# cycles of clock)         50Hz frequency, 20 ms period
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set mode 7
    TIMER_A2->CCR[1] = 3000; // CCR1 PWM duty cycle in 10ths of percent     (3000 = Door closed)
    TIMER_A2->CTL = 0x0254; // SMCLK, Up Mode, /2 divider, clear TAR to start

    P6->SEL0 |= BIT7;               //TimerA2 on P6.7 for LCD brightness
    P6->SEL1 &= ~(BIT7);
    P6->DIR |= BIT7; // P6.7 set TA2.4
    TIMER_A2->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set mode 7
    TIMER_A2->CCR[4] = (37500 * 0.8); // CCR2 PWM duty cycle in 10ths of percent
}
/****| StateMachine | *****************************************
 * Brief: This function is the main part of the program. It is the state
 *          machine that cycles through all of the different menus and
 *          associated actions
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void StateMachine() {
    int state = Main; // initial state
    int duty_cycle = 0;
    int pressed;
    P2->OUT |= BIT0;
    while (1) {
        switch (state) {
        case Main:
            lcdClear();
            lcdSetText("Select option:",0,0);
            lcdSetText("(1) Door",2,1);
            lcdSetText("(2) Motor",2,2);
            lcdSetText("(3) Lights",2,3);
            pressed = Read_Keypad ( ); // Call Function to read Keypad
            __delay_cycles(30000); // 10ms delay through the loop before reading keypad again
            if (key_flag == 1) {
                Print_Keys (pressed);
                if(pressed == 1) {
                    state = Door_menu; /*actions*/
                }
                else if(pressed == 2) {
                    state = Motor_menu; /*actions*/
                }
                else if(pressed == 3) {
                    state = Lights_menu; /*actions*/
                }
            }
            __delay_cycles(30000); // 10ms delay through the loop before reading keypad again
            break;
        case Door_menu:
            lcdClear();
            lcdSetText("Select option:",0,0);
            lcdSetText("(1) Open Door",2,1);
            lcdSetText("(2) Close door",2,2);
            pressed = Read_Keypad ( ); // Call Function to read Keypad
            if (key_flag == 1) {
                if(pressed == 1) {state = Door_open; /*actions*/ }
                else if(pressed == 2) {state = Door_closed; /*actions*/ }
            }
            __delay_cycles(30000); // 10ms delay through the loop before reading keypad again
            break;
        case Door_open:
            TIMER_A2->CCR[1] = 1000;
            P2->OUT &= ~(BIT0|BIT1);
            P2->OUT |= BIT1;  //green LED on
            state = Main;
            break;
        case Door_closed:
            TIMER_A2->CCR[1] = 3000;
            P2->OUT &= ~(BIT0|BIT1);
            P2->OUT |= BIT0; //red LED on
            state = Main;
            break;
        case Motor_menu:
            lcdClear();
            lcdSetText("Enter Speed 0-9",0,0);
            state = Motor;
            break;
        case Motor:
            pressed = Read_Keypad ( ); // Call Function to read Keypad
            if (key_flag == 1) {
                Print_Keys (pressed);
                duty_cycle = pressed;
                TIMER_A0->CCR[1] = (37500 * duty_cycle * 0.10);
                state = Main;
            }
            __delay_cycles(30000); // 10ms delay through the loop before reading keypad again
            break;
        case Lights_menu:
            lcdClear();
            lcdSetText("Select Color:",0,0);
            lcdSetText("(1) Red",2,1);
            lcdSetText("(2) Green",2,2);
            lcdSetText("(3) Blue",2,3);
            pressed = Read_Keypad ( ); // Call Function to read Keypad
            if (key_flag == 1) {
                Print_Keys (pressed);
                if(pressed == 1) {state = Red; /*actions*/ }
                else if(pressed == 2) {state = Green; /*actions*/ }
                else if(pressed == 3) {state = Blue; /*actions*/ }
            }
            __delay_cycles(30000); // 10ms delay through the loop before reading keypad again
            break;
        case Red:
            lcdClear();
            lcdSetText("Set Brightness:",1,0);
            int red[5];
            int count = 0;
            for (count = 0; count < 2; ){
                pressed = Read_Keypad ( ); // Call Function to read Keypad
                if (key_flag == 1) {
                    Print_Keys (pressed);
                    red[count] = pressed;
                    count++;
                }
                __delay_cycles(36000); // 12ms delay through the loop before reading keypad again
            }
            red_duty = ((red[0]*0.1)+(red[1]*0.01));
            TIMER_A0->CCR[2] = (37500 *red_duty);
            state = Main;
            break;
        case Green:
            lcdClear();
            lcdSetText("Set Brightness:",1,0);
            int green[5];
            for (count = 0; count < 2; ){
                pressed = Read_Keypad ( ); // Call Function to read Keypad
                if (key_flag == 1) {
                    Print_Keys (pressed);
                    green[count] = pressed;
                    count++;
                }
                __delay_cycles(36000); // 12ms delay through the loop before reading keypad again
            }
            green_duty = ((green[0]*0.1)+(green[1]*0.01));
            TIMER_A0->CCR[3] = (37500 *green_duty);
            state = Main;
            break;
        case Blue:
            lcdClear();
            lcdSetText("Set Brightness:",1,0);
            int blue[5];
            for (count = 0; count < 2; ){
                pressed = Read_Keypad ( ); // Call Function to read Keypad
                if (key_flag == 1) {
                    Print_Keys (pressed);
                    blue[count] = pressed;
                    count++;
                }
                __delay_cycles(36000); // 12ms delay through the loop before reading keypad again
            }
            blue_duty = ((blue[0]*0.1)+(blue[1]*0.01));
            TIMER_A2->CCR[2] = (37500 *blue_duty);
            state = Main;
            break;
        }
    }
}
/****| pininit | *****************************************
 * Brief: This function initializes red and green LED for the door
 *          as well as P5.0 and P5.1 for buttons with interrupts for
 *          the motor and LED enable/disable
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pininit(){
    //Rg LED are P2.0, P2.1
    P2->SEL0 &= ~(BIT0|BIT1);
    P2->SEL1 &= ~(BIT0|BIT1);  //clear select registers, selecting gpio
    P2->DIR |= (BIT0|BIT1);  //configuring pins as outputs
    //P2->REN   ren is only for inputs
    P2->OUT &= ~(BIT0|BIT1);  //LEDs are initally off
    P2->OUT |= BIT0;  //Red LED on, Door closed on start-up

    P5->SEL0 &=~ (BIT0|BIT1);
    P5->SEL1 &=~ (BIT0|BIT1);
    P5->DIR &=~ (BIT0|BIT1);
    P5->REN |= (BIT0|BIT1);  //pull up resistor
    P5->OUT |= (BIT0|BIT1); // Enable pull-up resistor
    P5->IES |= (BIT0|BIT1); //Set pin interrupt to trigger when it goes from high to low
    P5->IE |= (BIT0|BIT1); // Enable interrupt for P5.0
    P5->IFG = 0; // Clear all P interrupt flags
}
/****| PORT5_IRQHandler  | *****************************************
 * Brief: Handler function for port interrupts- button on P5.0 stops
 *          motor, button on P5.1 turns on or off LED lights
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void PORT5_IRQHandler(void) // Port5 ISR
{
    if ( P5->IFG & BIT0 ){  // If P5.0 had an interrupt (going from high to low)
        TIMER_A0->CCR[1] = (0);
    }
    else if ( P5->IFG & BIT1 ){  // If P5.1 had an interrupt (going from high to low)
        if(flag == 0){
            flag = 1;
            TIMER_A0->CCR[2] = (0);
            TIMER_A0->CCR[3] = (0);
            TIMER_A2->CCR[2] = (0);
        }
        else {
            flag = 0;
            TIMER_A0->CCR[2] = (37500 *red_duty);
            TIMER_A0->CCR[3] = (37500 *green_duty);
            TIMER_A2->CCR[2] = (37500 *blue_duty);
        }
    }
    P5->IFG = 0; // Clear all P1 interrupt flags
}
/****| adcsetup  | *****************************************
 * Brief: initialization of ADC on p5.5 starting in memory location 5
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void adcsetup(void)
{
    ADC14->CTL0 = 0x00000010; //power on and disabled during configuration
    ADC14->CTL0 |= 0x04D80300; // S/H pulse mode, MCLCK, 32 sample clocks, sw trigger, /4 clock divide
    ADC14->CTL1 = 0x00000030; // 14-bit resolution
    ADC14->MCTL[5] = 0; // A0 input, single ended, vref=avcc
    P5->SEL1 |= 0x20; // configure pin 5.5 for AO
    P5->SEL0 |= 0x20;
    ADC14->CTL1 |= 0x00050000; //start converting at mem reg 5
    ADC14->CTL0 |= 2; //enable ADC after configuration
}
/****| LCDbright  | *****************************************
 * Brief: This function updates the brightness of the LCD using PWM
 *          based on the ADC conversion of the potentiometer
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void LCDbright(void)
{
    float voltage;
    int result;
    ADC14->CTL0 |=1; //start a conversion
    while(!ADC14->IFGR0); // wait until conversion complete
    result = ADC14->MEM[5]; // immediately store value in a variable
    voltage = (result*0.000201);
    __delay_cycles(1500000); // wait for next value

    TIMER_A2->CCR[4] = (37500 * (result / 16384));
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    pininit();   // initialize red and green on board LED
    lcdInit();   // initialize LCD with defined pins (pins can be modified in LCD_Library.h)
    SysTickInit_NoInterrupts();   // initialize SysTick peripherals
    TimerA_init();
    keypad_init();
    NVIC_EnableIRQ(PORT5_IRQn);
    __enable_irq ();
    adcsetup();

    while (1){
        StateMachine();
    }
}
