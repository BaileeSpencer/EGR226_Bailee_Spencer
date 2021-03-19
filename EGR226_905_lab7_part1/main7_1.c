#include "msp.h"

/****| pinint  | *****************************************
 * Brief: initialization of pins used for the LCD
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void pinint(){

    P4->SEL0 &= ~(BIT0|BIT2|BIT4|BIT5|BIT6|BIT7);
    P4->SEL1 &= ~(BIT0|BIT2|BIT4|BIT5|BIT6|BIT7);  //clear select registers, selecting gpio
    P4->DIR |= (BIT0|BIT2|BIT4|BIT5|BIT6|BIT7);  //configuring pins as outputs
    //P4->REN   ren is only for inputs
    P4->OUT &= ~(BIT0|BIT2|BIT4|BIT5|BIT6|BIT7);

}

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
/****| Systick_ms_delay  | *****************************************
 * Brief: Systick delay function using ms
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void Systick_ms_delay (uint16_t delay) {
    SysTick -> LOAD = ((delay * 3000) - 1); //delay for 1 msecond per delay value
    SysTick -> VAL = 0; // any write to CVR clears it
    while ( (SysTick -> CTRL & 0x00010000) == 0); // wait for flag to be SET
}
/****| Systick_us_delay  | *****************************************
 * Brief: Systick delay function using ms
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void Systick_us_delay (uint16_t microsecond) {
    SysTick -> LOAD = ((microsecond * 3) - 1); //delay for 1 us per delay value
    SysTick -> VAL = 0; // any write to CVR clears it
    while ( (SysTick -> CTRL & 0x00010000) == 0); // wait for flag to be SET
}

/****| PulseEnablePin  | *****************************************
 * Brief: clears enable pin, delays, sets enable pin, delays, clears enable pin again, and then delays
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void PulseEnablePin (void) {
P4OUT &=~BIT2; // make sure pulse starts out at 0V
Systick_us_delay(10);
P4OUT |=BIT2;
Systick_us_delay(10);
P4OUT &=~BIT2;
Systick_us_delay(10);
}
/****| pushNibble  | *****************************************
 * Brief: pushes nibble to LCD and calls PulseEnablePin()
 * param:
 *      uint8_t nibble
 * return:
 *      n/a
 *************************************************************/
void pushNibble (uint8_t nibble) {
P4OUT &=~0xF0; // clear P4.4-P4.7
P4OUT |= (nibble & 0x0F) << 4; // port pins P4.4 - P4.7 wired to D4 - D7
PulseEnablePin();
}
/****| pushByte  | *****************************************
 * Brief: sends most significant 4 bits then least significant 4 bits to pushNibble()
 * param:
 *      uint8_t byte
 * return:
 *      n/a
 *************************************************************/
void pushByte (uint8_t byte) {
uint8_t nibble;
nibble = (byte & 0xF0) >> 4;
pushNibble(nibble);
nibble = byte & 0x0F;
pushNibble(nibble);
Systick_us_delay(100);
}
/****| write_command  | *****************************************
 * Brief: clears RS pin to prepare for sending instructions and then calls pushByte to send instructions
 * param:
 *      uint8_t command
 * return:
 *      n/a
 *************************************************************/
void write_command (uint8_t command){
    P4OUT &= ~BIT0;
    pushByte(command);
}
/****| Data_Write  | *****************************************
 * Brief: sets RS pin to prepare for writing data to LCD and then calls pushByte to send data
 * param:
 *      uint8_t data
 * return:
 *      n/a
 *************************************************************/
void Data_Write(uint8_t data){
    P4OUT |= BIT0;
    pushByte(data);
}
/****| LCD_init  | *****************************************
 * Brief: intializes and prepares LCD for future data or instructions
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void LCD_init(void){
     write_command(3);
     Systick_ms_delay(100);
     write_command(3);
     Systick_us_delay(200);
     write_command(3);
     Systick_ms_delay(100);
     write_command(2);
     Systick_us_delay(100);
     write_command(0x28);
     Systick_us_delay(100);
     Systick_us_delay(100);
     write_command(0x0F);
     Systick_us_delay(100);
     write_command(0x01);
     Systick_us_delay(100);
     write_command(0x06);
     Systick_ms_delay(10);
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	pinint();
	SysTick_Init ();
	LCD_init();

}
