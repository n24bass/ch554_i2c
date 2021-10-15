# CH554 i2c project for SDCC

This is Bit-bang i2c library and a example using some i2c devices. Some codes comes from "CH554 software development kit for SDCC'.

## Build a example

	$ cd projects/bmp180
	$ make
	$ make pyflash

It uses these i2c devices.

* BMP180 barometric pressure sensor
* ST7032i LCD driver

Default i2c pins (SCLK and SDAT) are P3_3 and P3_4. These GPIO ports can be defined by the symbol I2C_SCLK and I2C_SDAT. You'd initialize the ports to "open-drain output" and "pull-up", or prepare and external pull-up.

<image src="https://user-images.githubusercontent.com/281339/137496513-e6f1b852-1fc4-419b-87c4-c80899f28e00.jpg" width="480"/>

# References

* [CH554 software development kit for SDCC](https://github.com/Blinkinlabs/ch554_sdcc)
* [CH55x_python_flasher](https://github.com/hexeguitar/CH55x_python_flasher)
* [Official CH554 SDK (snapshot)](https://github.com/HonghongLu/CH554)
* [SDCC Compiler Users Guide](http://sdcc.sourceforge.net/doc/sdccman.pdf)
* [CH554 manual (Chinese)](https://github.com/HonghongLu/CH554/blob/master/CH554DS1.PDF)
* [CH554 manual (English-ish translation)](https://github.com/Blinkinlabs/ch554_sdcc/blob/master/documentation/CH554%20manual%20english.pdf)
* [CH554 instruction set timings](https://github.com/HonghongLu/CH554/blob/master/official%20ch554%20evt%20pcb/PUB/CH55X%C2%AA%E2%80%9E%C2%B1%E2%80%A1%C3%B7%E2%88%8F%C2%A1%C3%93.PDF)

