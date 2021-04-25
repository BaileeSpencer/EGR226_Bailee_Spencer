/**************************************************************************************
*                                LCD_CustomImages.c
*                                Trevor Ekin
*                         EGR226      Date: February-21-2019
*
*  This file contains all of the custom characters to be used on the LCD.
* **************************************************************************************/

#ifndef LCD_CUSTOMIMAGES_H_
#define LCD_CUSTOMIMAGES_H_

//Include LCD library to get custom_char_t struct
#include "LCD_Library.h"

// custom_char_t is a struct containing 8 bytes of data, representing the
//      8 rows of dots that create an LCD character. Each bit is either
//      on (1) or off (0) to display the image you desire.
// you can make a custom character at (https://omerk.github.io/lcdchargen/)

// example custom character struct of a bell:
custom_char_t bell_layout = {
  0b00100,           // line 0 of the bell
  0b00100,           // line 1 of the bell
  0b01110,           // line 2 of the bell
  0b01110,           // line 3 of the bell
  0b01110,           // line 4 of the bell
  0b11111,           // line 5 of the bell
  0b00100,           // line 6 of the bell
  0b00000            // line 7 of the bell
};

// example custom character struct of a stick figure:
custom_char_t stickfig_layout = {
  0b00100,           // line 0 of the stick figure
  0b01110,           // line 1 of the stick figure
  0b01110,           // line 2 of the stick figure
  0b00100,           // line 3 of the stick figure
  0b11111,           // line 4 of the stick figure
  0b00100,           // line 5 of the stick figure
  0b01010,           // line 6 of the stick figure
  0b01010            // line 7 of the stick figure
};

// example custom character struct of a smile:
custom_char_t smile_layout = {
  0b00000,        // line 0 of the smile face
  0b01010,        // line 1 of the smile face
  0b01010,        // line 2 of the smile face
  0b00000,        // line 3 of the smile face
  0b00100,        // line 4 of the smile face
  0b10001,        // line 5 of the smile face
  0b10001,        // line 6 of the smile face
  0b01110         // line 7 of the smile face
};

#endif /* LCD_CUSTOMIMAGES_H_ */
