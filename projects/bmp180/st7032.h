#ifndef __ST7032_H__
#define __ST7032_H__

#include <ch554.h>
#include <debug.h>

#include <stdint.h>
#include <stdio.h>

#include "i2c.h"

#define ST7032_I2C_DEFAULT_ADDR     0x3E // 7bit, 0x7C for 8bit

// commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

#define LCD_EX_SETBIASOSC       0x10        // Bias selection / Internal OSC frequency adjust
#define LCD_EX_SETICONRAMADDR   0x40        // Set ICON RAM address
#define LCD_EX_POWICONCONTRASTH 0x50        // Power / ICON control / Contrast set(high byte)
#define LCD_EX_FOLLOWERCONTROL  0x60        // Follower control
#define LCD_EX_CONTRASTSETL     0x70        // Contrast set(low byte)

// flags for display entry mode
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// flags for function set
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00
#define LCD_EX_INSTRUCTION      0x01        // IS: instruction table select

// flags for Bias selection
#define LCD_BIAS_1_4            0x08        // bias will be 1/4
#define LCD_BIAS_1_5            0x00        // bias will be 1/5

// flags Power / ICON control / Contrast set(high byte)
#define LCD_ICON_ON             0x08        // ICON display on
#define LCD_ICON_OFF            0x00        // ICON display off
#define LCD_BOOST_ON            0x04        // booster circuit is turn on
#define LCD_BOOST_OFF           0x00        // booster circuit is turn off
#define LCD_OSC_122HZ           0x00        // 122Hz@3.0V
#define LCD_OSC_131HZ           0x01        // 131Hz@3.0V
#define LCD_OSC_144HZ           0x02        // 144Hz@3.0V
#define LCD_OSC_161HZ           0x03        // 161Hz@3.0V
#define LCD_OSC_183HZ           0x04        // 183Hz@3.0V
#define LCD_OSC_221HZ           0x05        // 221Hz@3.0V
#define LCD_OSC_274HZ           0x06        // 274Hz@3.0V
#define LCD_OSC_347HZ           0x07        // 347Hz@3.0V

// flags Follower control
#define LCD_FOLLOWER_ON         0x08        // internal follower circuit is turn on
#define LCD_FOLLOWER_OFF        0x00        // internal follower circuit is turn off
#define LCD_RAB_1_00            0x00        // 1+(Rb/Ra)=1.00
#define LCD_RAB_1_25            0x01        // 1+(Rb/Ra)=1.25
#define LCD_RAB_1_50            0x02        // 1+(Rb/Ra)=1.50
#define LCD_RAB_1_80            0x03        // 1+(Rb/Ra)=1.80
#define LCD_RAB_2_00            0x04        // 1+(Rb/Ra)=2.00
#define LCD_RAB_2_50            0x05        // 1+(Rb/Ra)=2.50
#define LCD_RAB_3_00            0x06        // 1+(Rb/Ra)=3.00
#define LCD_RAB_3_75            0x07        // 1+(Rb/Ra)=3.75

extern void lcdInit(uint8_t cols, uint8_t lines, uint8_t dotsize);

// void setWire(TwoWire* wire);
extern void lcdSetContrast(uint8_t cont);
extern void lcdSetIcon(uint8_t addr, uint8_t bit);
extern void lcdClear();
extern void lcdHome();

extern void lcdNoDisplay();
extern void lcdDisplay();
extern void lcdNoBlink();
extern void lcdBlink();
extern void lcdNoCursor();
extern void lcdCursor();
extern void lcdScrollDisplayLeft();
extern void lcdScrollDisplayRight();
extern void lcdLeftToRight();
extern void lcdRightToLeft();
extern void lcdAutoscroll();
extern void lcdNoAutoscroll();

extern void lcdCreateChar(uint8_t location, uint8_t charmap[]);
extern void lcdSetCursor(uint8_t col, uint8_t row);
extern void lcdPrint(char *s);

/* virtual size_t write(uint8_t value); */
extern size_t lcdData(uint8_t value);
extern void lcdCommand(uint8_t value);

#endif // __ST7032_H__
