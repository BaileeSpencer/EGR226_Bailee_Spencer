#include "msp.h"
#include <stdio.h>

int flag_C;

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

/****| adcsetup  | *****************************************
 * Brief: initialization of ADC on p5.5 starting in memory location 5
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
void adcsetup(void) {
    ADC14->CTL0 = 0x00000010; //power on and disabled during configuration
    ADC14->CTL0 |= 0x04D80300; // S/H pulse mode, MCLCK, 32 sample clocks, sw trigger, /4 clock divide
    ADC14->CTL1 = 0x00000030; // 14-bit resolution
    ADC14->MCTL[5] = 0; // A0 input, single ended, vref=avcc
    P5->SEL1 |= 0x20; // configure pin 5.5 for AO
    P5->SEL0 |= 0x20;
    ADC14->CTL1 |= 0x00050000; //start converting at mem reg 5
    ADC14->CTL0 |= 2; //enable ADC after configuration
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
    SysTick -> LOAD = 0x00FFFFFF; // max reload value
    SysTick -> VAL = 0; // any write to current clears it
    SysTick -> CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}

/****| Port5_init  | *****************************************
 * Brief: initialization of pins P5.0 to be used for buttons with port interrupts
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
    if(flag_C == 1){
        flag_C = 0;
    }
    else {
        flag_C = 1;
    }
    P5->IFG = 0; // Clear all P1 interrupt flags
}
/****| convertTemp  | *****************************************
 * Brief: uses ADC converter to print out temperature
 * param:
 *      n/a
 * return:
 *      n/a
 *************************************************************/
float convertTemp (void){
    int result;
    float voltage, celsius, Fahr;
    while (1){
        ADC14->CTL0 |=1; //start a conversion
        while(!ADC14->IFGR0); // wait until conversion complete
        result = ADC14->MEM[5]; // immediately store value in a variable
        voltage = (result*0.000201);
        celsius = ((voltage * 1000) - 500) / 10;
        Fahr = (celsius * 1.8) + 32;
        if (flag_C == 1){
            return (celsius);
        }
        else{
            return Fahr;
        }
    }
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
/****| print_string  | *****************************************
 * Brief: takes a 10 character string and displays it to the LCD screen
 * param:
 *      char str[10]
 * return:
 *      n/a
 *************************************************************/
void print_string(char str[20]){
    int i;
    for (i=0; (str[i] != '\0'); i++){
        if (str[i] == 'A'){
            Data_Write(0x41);
        }
        else if (str[i] == 'B'){
            Data_Write(0x42);
        }
        else if (str[i] == 'C'){
            Data_Write(0x43);
        }
        else if (str[i] == 'D'){
            Data_Write(0x44);
        }
        else if (str[i] == 'E'){
            Data_Write(0x45);
        }
        else if (str[i] == 'F'){
            Data_Write(0x46);
        }
        else if (str[i] == 'G'){
            Data_Write(0x47);
        }
        else if (str[i] == 'H'){
            Data_Write(0x48);
        }
        else if (str[i] == 'I'){
            Data_Write(0x49);
        }
        else if (str[i] == 'J'){
            Data_Write(0x4A);
        }
        else if (str[i] == 'K'){
            Data_Write(0x4B);
        }
        else if (str[i] == 'L'){
            Data_Write(0x4C);
        }
        else if (str[i] == 'M'){
            Data_Write(0x4D);
        }
        else if (str[i] == 'N'){
            Data_Write(0x4E);
        }
        else if (str[i] == 'O'){
            Data_Write(0x4F);
        }
        else if (str[i] == 'P'){
            Data_Write(0x50);
        }
        else if (str[i] == 'Q'){
            Data_Write(0x51);
        }
        else if (str[i] == 'R'){
            Data_Write(0x52);
        }
        else if (str[i] == 'S'){
            Data_Write(0x53);
        }
        else if (str[i] == 'T'){
            Data_Write(0x54);
        }
        else if (str[i] == 'U'){
            Data_Write(0x55);
        }
        else if (str[i] == 'V'){
            Data_Write(0x56);
        }
        else if (str[i] == 'W'){
            Data_Write(0x57);
        }
        else if (str[i] == 'X'){
            Data_Write(0x58);
        }
        else if (str[i] == 'Y'){
            Data_Write(0x59);
        }
        else if (str[i] == 'Z'){
            Data_Write(0x5A);
        }
        else if (str[i] == 'a'){
            Data_Write(0x61);
        }
        else if (str[i] == 'b'){
            Data_Write(0x62);
        }
        else if (str[i] == 'c'){
            Data_Write(0x63);
        }
        else if (str[i] == 'd'){
            Data_Write(0x64);
        }
        else if (str[i] == 'e'){
            Data_Write(0x65);
        }
        else if (str[i] == 'f'){
            Data_Write(0x66);
        }
        else if (str[i] == 'g'){
            Data_Write(0x67);
        }
        else if (str[i] == 'h'){
            Data_Write(0x68);
        }
        else if (str[i] == 'i'){
            Data_Write(0x69);
        }
        else if (str[i] == 'j'){
            Data_Write(0x6A);
        }
        else if (str[i] == 'k'){
            Data_Write(0x6B);
        }
        else if (str[i] == 'l'){
            Data_Write(0x6C);
        }
        else if (str[i] == 'm'){
            Data_Write(0x6D);
        }
        else if (str[i] == 'n'){
            Data_Write(0x6E);
        }
        else if (str[i] == 'o'){
            Data_Write(0x6F);
        }
        else if (str[i] == 'p'){
            Data_Write(0x70);
        }
        else if (str[i] == 'q'){
            Data_Write(0x71);
        }
        else if (str[i] == 'r'){
            Data_Write(0x72);
        }
        else if (str[i] == 's'){
            Data_Write(0x73);
        }
        else if (str[i] == 't'){
            Data_Write(0x74);
        }
        else if (str[i] == 'u'){
            Data_Write(0x75);
        }
        else if (str[i] == 'v'){
            Data_Write(0x76);
        }
        else if (str[i] == 'w'){
            Data_Write(0x77);
        }
        else if (str[i] == 'x'){
            Data_Write(0x78);
        }
        else if (str[i] == 'y'){
            Data_Write(0x79);
        }
        else if (str[i] == 'z'){
            Data_Write(0x7A);
        }
        else if (str[i] == ' '){
            Data_Write(0x20);
        }
        else if (str[i] == '0'){
            Data_Write(0x30);
        }
        else if (str[i] == '1'){
            Data_Write(0x31);
        }
        else if (str[i] == '2'){
            Data_Write(0x32);
        }
        else if (str[i] == '3'){
            Data_Write(0x33);
        }
        else if (str[i] == '4'){
            Data_Write(0x34);
        }
        else if (str[i] == '5'){
            Data_Write(0x35);
        }
        else if (str[i] == '6'){
            Data_Write(0x36);
        }
        else if (str[i] == '7'){
            Data_Write(0x37);
        }
        else if (str[i] == '8'){
            Data_Write(0x38);
        }
        else if (str[i] == '9'){
            Data_Write(0x39);
        }
        else if (str[i] == '.'){
            Data_Write(0x2E);
        }
    }
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    adcsetup();
    SysTick_Init();
    pinint();
    LCD_init();
    Port5_init();
    NVIC_EnableIRQ(PORT5_IRQn);
    __enable_irq();
    flag_C = 1;

    float num;

    char word[18] = "Current Temp is";
    print_string(word);

    while(1){
        write_command(0x80|0x45);
        num = convertTemp();
        sprintf(word, "%0.1f", num);
        print_string(word);
        Data_Write(0xDF);
        __delay_cycles(1500000);
    }
}
