TARGET = bmp180

FREQ_SYS=16000000
EXTRA_FLAGS = -DMAX_PACKET_SIZE=64 -DI2C_SDAT=P1_5 -DI2C_SCLK=P1_4

XRAM_SIZE = 0x380
XRAM_LOC = 0x080

C_FILES = \
	main.c \
	st7032.c \
	bmp180.c \
	../../common/i2c.c \
	../../common/debug.c

pre-flash:

pyflash:
	chflasher.py -f $(TARGET).bin -w -s

include ../Makefile.include
