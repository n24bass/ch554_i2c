// Blink an LED connected to pin 1.7

#include <8051.h>
#include <ch554.h>
#include <debug.h>
#include "i2c.h"
#include "st7032.h"

#define LED_PIN 7
#define LED P1_7

#define I2C_ADDR 0xc4

extern void init_bmp180();
extern int16_t get_temp();
extern uint16_t get_pressure();


void main() {
  char buf[20];
  int16_t t;
  uint16_t p;
	
  CfgFsys();
  mInitSTDIO();
  i2c_init();

#if DEBUG
  printf("start\r\n");
#endif

  // Configure pin 1.7 as GPIO output - push/pull output
  P1_MOD_OC = P1_MOD_OC & ~(1<<LED_PIN); // push/pull out
  lcdInit(16, 2, LCD_5x8DOTS);

  lcdClear();
  mDelaymS(500);
  lcdPrint("Hello i2c");

  init_bmp180();
  mDelaymS(100);
		
  // int i = 0;
  while (1) {
    LED = !LED;

    t = get_temp();
    p = get_pressure();
    sprintf(buf, "%2d.%ddegC %4dhPa", t/10, t % 10, p);
#if DEBUG
    printf("%s\r\n", buf);
#endif
    lcdSetCursor(15, 0);
    lcdData(LED? '*': ' ');
    lcdSetCursor(0, 1);
    lcdPrint(buf);

    mDelaymS(500);
  }
}
