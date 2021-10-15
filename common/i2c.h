/********************************** (C) COPYRIGHT *******************************
* File Name		: I2C.H
* Author		: Masato YAMANISHI
* License		: MIT
* Version		: V1.0
* Date			: 2021/10/13
* Description		: 8051 software I2C
*******************************************************************************/
#ifndef _I2C_H_

#define _I2C_H_

extern void i2c_init();
extern uint8_t i2c_write(uint8_t addr, uint8_t *p, uint8_t len);
extern uint8_t i2c_read(uint8_t addr, uint8_t *p, uint8_t len);

#endif
