#include <8051.h>
#include <ch554.h>
#include <debug.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "i2c.h"

#define BMP180_ADDR 0x77 // 0xEE // 0x77

#define BMP085_ULTRALOWPOWER 0 //!< Ultra low power mode
#define BMP085_STANDARD 1      //!< Standard mode
#define BMP085_HIGHRES 2       //!< High-res mode
#define BMP085_ULTRAHIGHRES 3  //!< Ultra high-res mode

#define BMP085_CAL_AC1 0xAA    //!< R   Calibration data (16 bits)
#define BMP085_CAL_AC2 0xAC    //!< R   Calibration data (16 bits)
#define BMP085_CAL_AC3 0xAE    //!< R   Calibration data (16 bits)
#define BMP085_CAL_AC4 0xB0    //!< R   Calibration data (16 bits)
#define BMP085_CAL_AC5 0xB2    //!< R   Calibration data (16 bits)
#define BMP085_CAL_AC6 0xB4    //!< R   Calibration data (16 bits)
#define BMP085_CAL_B1 0xB6     //!< R   Calibration data (16 bits)
#define BMP085_CAL_B2 0xB8     //!< R   Calibration data (16 bits)
#define BMP085_CAL_MB 0xBA     //!< R   Calibration data (16 bits)
#define BMP085_CAL_MC 0xBC     //!< R   Calibration data (16 bits)
#define BMP085_CAL_MD 0xBE     //!< R   Calibration data (16 bits)

#define BMP085_CONTROL 0xF4         //!< Control register
#define BMP085_TEMPDATA 0xF6        //!< Temperature data register
#define BMP085_PRESSUREDATA 0xF6    //!< Pressure data register
#define BMP085_READTEMPCMD 0x2E     //!< Read temperature control register value
#define BMP085_READPRESSURECMD 0x34 //!< Read pressure control register value

static  int16_t ac1, ac2, ac3, b1, b2, mb, mc, md; // 
static  uint16_t ac4, ac5, ac6; // 

// #define DEBUG

static uint8_t read8(uint8_t a) {
  uint8_t x;

  i2c_write(BMP180_ADDR, &a, 1);
  i2c_read(BMP180_ADDR, &x, 1);

  return x;
}

static uint16_t read16(uint8_t a) {
  uint8_t b[2];

  i2c_write(BMP180_ADDR, &a, 1);
  i2c_read(BMP180_ADDR, b, 2);
  
  return(b[0] << 8 | b[1]);
}

int32_t computeB5(int32_t UT) {
  int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
  int32_t X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md);
  return X1 + X2;
}
	
void init_bmp180() {
#ifdef DEBUG
  printf("init_bmp180:%x\r\n", read8(0xD0));
#endif
  ac1 = read16(BMP085_CAL_AC1);
  ac2 = read16(BMP085_CAL_AC2);
  ac3 = read16(BMP085_CAL_AC3);
  ac4 = read16(BMP085_CAL_AC4);
  ac5 = read16(BMP085_CAL_AC5);
  ac6 = read16(BMP085_CAL_AC6);

  b1 = read16(BMP085_CAL_B1);
  b2 = read16(BMP085_CAL_B2);

  mb = read16(BMP085_CAL_MB);
  mc = read16(BMP085_CAL_MC);
  md = read16(BMP085_CAL_MD);
#ifdef DEBUG
  printf("ac1=%d\r\n", ac1);
  printf("ac2=%d\r\n", ac2);
  printf("ac3=%d\r\n", ac3);
  printf("ac4=%d\r\n", ac4);
  printf("ac5=%d\r\n", ac5);
  printf("ac6=%d\r\n", ac6);
  printf("b1=%d\r\n", b1);
  printf("b2=%d\r\n", b2);
  printf("mb=%d\r\n", mb);
  printf("mc=%d\r\n", mc);
  printf("md=%d\r\n", md);
#endif
}

static int16_t get_rawtemp() {
  uint8_t b[2];

  b[0] = BMP085_CONTROL;
  b[1] = BMP085_READTEMPCMD;
  i2c_write(BMP180_ADDR, b, 2);
  mDelaymS(10);
  
  return read16(BMP085_TEMPDATA); // raw temp
}

int16_t get_temp() {
  int32_t UT, B5, temp;
	
  UT = get_rawtemp();
  B5 = (int32_t)computeB5(UT);
  temp = (B5 + 8) >> 4;
#ifdef DEBUG
  printf("UT:%d, temp:%d\r\n", (int)UT, (int16_t)temp);
#endif
  return (int16_t)temp;
}

uint32_t get_rawpressure() {
  uint8_t b[2];
  uint32_t raw;

  b[0] = BMP085_CONTROL;
  b[1] = BMP085_READPRESSURECMD + BMP085_ULTRALOWPOWER;
  i2c_write(BMP180_ADDR, b, 2);
  mDelaymS(8);

  /*
    if (oversampling == BMP085_ULTRALOWPOWER)
    delay(5);
    else if (oversampling == BMP085_STANDARD)
    delay(8);
    else if (oversampling == BMP085_HIGHRES)
    delay(14);
    else
    delay(26);
  */

  raw = read16(BMP085_PRESSUREDATA);

  raw <<= 8;
  raw |= read8((uint8_t)(BMP085_PRESSUREDATA + 2));
  raw >>= (8 - BMP085_ULTRALOWPOWER); // (8 - oversampling);
#ifdef DEBUG
  printf("Raw pressure: %ld\r\n", (long)raw);
#endif
  return raw;
}

uint16_t get_pressure() {
  int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
  uint32_t B4, B7;

  UT = get_rawtemp();
  UP = get_rawpressure();

  B5 = computeB5(UT);
  // do pressure calcs
  B6 = B5 - 4000;
  X1 = ((int32_t)b2 * ((B6 * B6) >> 12)) >> 11;
  X2 = ((int32_t)ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t)ac1 * 4 + X3) << BMP085_ULTRALOWPOWER) + 2) / 4;
#ifdef DEBUG
  printf("--\r\nUP=%ld, B6=%ld, X1=%ld, X2=%ld, B3=%ld\r\n",
	 (long)UP, (long)B6, (long)X1, (long)X2, (long)B3);
#endif
  X1 = ((int32_t)ac3 * B6) >> 13;
  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  B7 = ((uint32_t)UP - B3) * (uint32_t)(50000UL >> BMP085_ULTRALOWPOWER);
#ifdef DEBUG
  printf("X1=%ld, X2=%ld, X3=%ld, B4=%lu, B7=%lu\r\n",
	 (long)X1, (long)X2, (long)X3, (long)B4, (long)B7);
#endif
  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;
#ifdef DEBUG
  printf("p=%ld, X1=%ld, X2=%ld\r\n", (long)p, (long)X1, (long)X2);
#endif
  p = p + ((X1 + X2 + (int32_t)3791) >> 4);
  p /= 100;
#ifdef DEBUG
  printf("p=%ld\r\n\r\n", (long)p);
#endif
  return (uint16_t)p;
}

