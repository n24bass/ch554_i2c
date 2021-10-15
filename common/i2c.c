/********************************** (C) COPYRIGHT *******************************
* File Name   : I2C.C
* Author    : Masato YAMANISHI
* License   : MIT
* Version   : V1.0
* Date      : 2021/10/13
* Description   : 8051 Software I2C
*******************************************************************************/

#include <stdint.h>
#include <8051.h>
#include <ch554.h>
#include "debug.h"
#include "i2c.h"

#ifdef I2C_DEFAULT
#define I2C_SDAT  P3_4
#define I2C_SCLK  P3_3
#endif

#ifndef I2C_SDAT
#define I2C_SDAT  P3_4
#endif

#ifndef I2C_SCLK
#define I2C_SCLK  P3_3
#endif

#define SDA_HIGH()  (I2C_SDAT = 1)
#define SDA_LOW() (I2C_SDAT = 0)
#define SDA_READ()  (I2C_SDAT)

#define SCL_HIGH()  (I2C_SCLK = 1)
#define SCL_LOW() (I2C_SCLK = 0)

void i2c_init()
{ /* GPIO port initial */
  SDA_HIGH();
  SCL_HIGH();
}

static void i2c_delay()
{
#if 1
  mDelayuS(8); /* 5 : about 100kHz */
#else
  int i = 1;
  while (i--)
    ;
#endif
}

static uint8_t i2c_byte_out(uint8_t b) {
  uint8_t i;
  
  for (i = 0; i < 8; i++) {
    (b & 0x80)? SDA_HIGH(): SDA_LOW();
    SCL_HIGH();
    i2c_delay();
    SCL_LOW();
    b <<= 1;
    i2c_delay();
  }

  /* read ack/nack */
  SDA_HIGH(); /* pull up SDA for read */
  SCL_HIGH();
  i2c_delay();
  i = SDA_READ(); /* read ack/nack */

  SCL_LOW();
  SDA_LOW();
  i2c_delay();

  return i? 0: 1; /* 1:ack, 0:nack */
}

static uint8_t i2c_byte_in(uint8_t last) {
  uint8_t i;
  uint8_t b = 0;

  SDA_HIGH(); /* pull up for input */
  for (i = 0; i < 8; i++) {
    i2c_delay();
    SCL_HIGH();
    b <<= 1;
    if (SDA_READ()) b |= 1;
    i2c_delay();
    SCL_LOW();
  }
  i2c_delay();
  last? SDA_HIGH(): SDA_LOW(); /* send nack or ack */
  SCL_HIGH();
  i2c_delay();
  SCL_LOW(); 
  SDA_LOW();
  i2c_delay();

  return b;
}

static void i2c_end() {
  /* stop condition */
  // SDA_LOW();
  // i2c_delay();
  SCL_HIGH();
  i2c_delay();
  SDA_HIGH();
  i2c_delay();
}

/* start consition, send address + RW */
static uint8_t i2c_begin(uint8_t addr, uint8_t rw) {
  /* start condition */
  SDA_LOW();
  i2c_delay();
  SCL_LOW();
  i2c_delay();
  
  /* address + read/write bit */
  addr <<= 1; /* shift addess */
  if (rw) addr++; /* read/write */

  /* send */
  return i2c_byte_out(addr);
}

uint8_t i2c_write(uint8_t addr, uint8_t *p, uint8_t len) {
  uint8_t b;
  uint8_t n = len;
  uint8_t rc = i2c_begin(addr, 0); /* start, set addr with WRITE bit */
  if (rc) {
    while (n && rc) {
      b = *p;
      rc = i2c_byte_out(b); // 0:ack, 1:nack
      if (rc) { // 
        n--;
        p++;
      }
    }
    rc = rc? ((len - n)? 1: 0): 0;
  }
  i2c_end();

  return rc;
}

uint8_t i2c_read(uint8_t addr, uint8_t *p, uint8_t len) {
  uint8_t rc = i2c_begin(addr, 1); /* start, set addr with READ bit */
  if (rc) {
    while (len--) {
      *p++ = i2c_byte_in(len == 0);
    }
  }
  i2c_end();
  return rc;
}

/* EOF */
