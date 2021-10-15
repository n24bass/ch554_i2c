#include "st7032.h"

static void setDisplayControl(uint8_t setBit);
static void resetDisplayControl(uint8_t resetBit);
static void setEntryMode(uint8_t setBit);
static void resetEntryMode(uint8_t resetBit);
static void normalFunctionSet();
static void extendFunctionSet();

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;

uint8_t _numlines;

/* private methods */

static void setDisplayControl(uint8_t setBit) {
  _displaycontrol |= setBit;
  lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

static void resetDisplayControl(uint8_t resetBit) {
  _displaycontrol &= ~resetBit;
  lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

static void setEntryMode(uint8_t setBit) {
  _displaymode |= setBit;
  lcdCommand(LCD_ENTRYMODESET | _displaymode);
}

static void resetEntryMode(uint8_t resetBit) {
  _displaymode &= ~resetBit;
  lcdCommand(LCD_ENTRYMODESET | _displaymode);
}

static void normalFunctionSet() {
  lcdCommand(LCD_FUNCTIONSET | _displayfunction);
}

static void extendFunctionSet() {
  lcdCommand(LCD_FUNCTIONSET | _displayfunction | LCD_EX_INSTRUCTION);
}

/* public methods */

/* ST7032_asukiaaa::ST7032_asukiaaa(int i2c_addr) */
void lcdInit(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  /* uint8_t cols = 16; */
  /* uint8_t lines = 2; */
  /* uint8_t dotsize = LCD_5x8DOTS; */

  _displaycontrol = 0x00;
  _displaymode = 0x00;
  // _i2c_addr = ST7032_I2C_DEFAULT_ADDR;

  _displayfunction  = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  // _currline = 0;

  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  /* finally, set # lines, font size, etc. */
  normalFunctionSet();

  extendFunctionSet();
  lcdCommand(LCD_EX_SETBIASOSC | LCD_BIAS_1_5 | LCD_OSC_183HZ); /* 1/5bias, OSC=183Hz@3.0V */
  lcdCommand(LCD_EX_FOLLOWERCONTROL | LCD_FOLLOWER_ON | LCD_RAB_2_00);     // internal follower circuit is turn on
  mDelaymS(200); // delay(200);                                       // Wait time >200ms (for power stable)
  normalFunctionSet();

  // turn the display on with no cursor or blinking default
  // display();
  _displaycontrol   = 0x00; //LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  setDisplayControl(LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);

  // clear it off
  lcdClear();

  // Initialize to default text direction (for romance languages)
  // lcdCommand(LCD_ENTRYMODESET | _displaymode);
  _displaymode  = 0x00; //LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  setEntryMode(LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

  lcdSetContrast(40);
}

void lcdSetContrast(uint8_t cont)
{
  extendFunctionSet();
  lcdCommand(LCD_EX_CONTRASTSETL | (cont & 0x0f));   /* Contrast set */
  lcdCommand(LCD_EX_POWICONCONTRASTH | LCD_ICON_ON | LCD_BOOST_ON | ((cont >> 4) & 0x03)); // Power, ICON, Contrast control
  normalFunctionSet();
}

void lcdSetIcon(uint8_t addr, uint8_t bit) {
  extendFunctionSet();
  lcdCommand(LCD_EX_SETICONRAMADDR | (addr & 0x0f)); /* ICON address */
  lcdData(bit); // write(bit);
  normalFunctionSet();
}

/********** high level commands, for the user! */
void lcdClear()
{
  lcdCommand(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  mDelayuS(2000); // delayMicroseconds(2000);  // this command takes a long time!
}

void lcdHome()
{
  lcdCommand(LCD_RETURNHOME);  // set cursor position to zero
  mDelayuS(2000); // delayMicroseconds(2000);  // this command takes a long time!
}

void lcdSetCursor(uint8_t col, uint8_t row)
{
  const int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }

  normalFunctionSet();
  lcdCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
  mDelayuS(2000);
  /* lcdCommand(LCD_SETDDRAMADDR | (col + row_offsets[row])); */
  /* mDelayuS(2000); */
}

// Turn the display on/off (quickly)
void lcdNoDisplay() {
  resetDisplayControl(LCD_DISPLAYON);
}
void lcdDisplay() {
  setDisplayControl(LCD_DISPLAYON);
}

// Turns the underline cursor on/off
void lcdNoCursor() {
  resetDisplayControl(LCD_CURSORON);
}
void lcdCursor() {
  setDisplayControl(LCD_CURSORON);
}

// Turn on and off the blinking cursor
void lcdNoBlink() {
  resetDisplayControl(LCD_BLINKON);
}
void lcdBlink() {
  setDisplayControl(LCD_BLINKON);
}

// These commands scroll the display without changing the RAM
void lcdScrollDisplayLeft(void) {
  lcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcdScrollDisplayRight(void) {
  lcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcdLeftToRight(void) {
  setEntryMode(LCD_ENTRYLEFT);
}

// This is for text that flows Right to Left
void lcdRightToLeft(void) {
  resetEntryMode(LCD_ENTRYLEFT);
}

// This will 'right justify' text from the cursor
void lcdAutoscroll(void) {
  setEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// This will 'left justify' text from the cursor
void lcdNoAutoscroll(void) {
  resetEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcdCreateChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  lcdCommand(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    lcdData(charmap[i]); // write(charmap[i]);
  }
}

void lcdPrint(char *s) {
  while (*s) {
    lcdData((uint8_t)*s);
    s++;
  }
}

/*********** mid level commands, for sending data/cmds */

void lcdCommand(uint8_t value) {
  uint8_t b[2];
  b[0] = 0x00;
  b[1] = value;
  (void)i2c_write(ST7032_I2C_DEFAULT_ADDR, b, 2);
  mDelayuS(27); // delayMicroseconds(27);    // >26.3us
}

size_t lcdData(uint8_t value) {
  uint8_t b[2];
  b[0] = 0x40;
  b[1] = value;
  (void)i2c_write(ST7032_I2C_DEFAULT_ADDR, b, 2);
  return 1;
}
