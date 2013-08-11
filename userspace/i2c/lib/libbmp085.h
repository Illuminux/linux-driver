/**
 * \mainpage
 *
 * libbmp085.h is a communication library for BOSCH Barometric Pressure
 * and Temperature Sensor.
 *
 * From user space you will need the SMBus level access helper functions
 * written by Simon G. Vogl, Frodo Looijaard and Jean Delvare.
 *
 * \copyright 2013 Knut Welzel (knut.welzels@googlemail.com)
 *
 */
/*  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Dieses Programm ist Freie Software: Sie k�nnen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder sp�teren
 *  ver�ffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  Dieses Programm wird in der Hoffnung, dass es n�tzlich sein wird, aber
 *  OHNE JEDE GEW�HRLEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gew�hrleistung der MARKTF�HIGKEIT oder EIGNUNG F�R EINEN BESTIMMTEN ZWECK.
 *  Siehe die GNU General Public License f�r weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBBMP085_H_
#define LIBBMP085_H_

/*
 * BMP085 Over sampling Modes
 */
#define BMP085_OVERSAMPLING_LOW      0  ///< Low over sampling mode
#define BMP085_OVERSAMPLING_STANDARD 1  ///< Default over sampling mode
#define BMP085_OVERSAMPLING_HIGH     2  ///< High over sampling mode
#define BMP085_OVERSAMPLING_ULTRA    3  ///< Ultra high over sampling mode


void bmp085_setup(__u8 i2c_device, __u8 i2c_address, unsigned char oversampling);
static inline void bmb085_get_calibration_parameter();
static inline void bmp085_i2c_write_byte(int fd, __u8 addr, __u8 value);

static inline unsigned int bmp085_get_ut();
static inline unsigned int bmp085_get_up();

struct bmp085_value bmp085_get_values();
float bmp085_get_pressure(void);
float bmp085_get_temperature(void);
float bmp085_get_altitude(float pressure);

static inline __s32 bmp085_i2c_read_int(int fd, __u8 reg_no);



/* TYPE DEFINITIONS */

/**
 * \brief Measurement values
 * @author Knut Welzel
 * @param  temperature - The temperature in deg C as float
 * @param  pressure    - The pressure in mbar as float
 * @param  altitude     - THe altitude in meter as float
 */
struct bmp085_value {
	float temperature;
	float pressure;
	float altitude;
};


/**
 * \brief Calibration values
 * \brief These values are stored in the EEPROM of the BMP085 sensor.
 * @author Knut Welzel
 */
struct bmb085_calibration {
	short int ac1;
	short int ac2;
	short int ac3;
	unsigned short int ac4;
	unsigned short int ac5;
	unsigned short int ac6;
	short int b1;
	short int b2;
	short int mb;
	short int mc;
	short int md;
	int b5;
};

struct bmb085_calibration bmb085_calibration;


/* RUNTIME VARIABLES */

/**
 * BMP085 i2c device number
 * \em 0: /dev/i2c-0
 * \em 1: /dev/i2c-1
 * @author Knut Welzel
 */
__u8 bmp085_i2c_device = 1;


/**
 * BMP085 i2c bus address.
 * \note The default address is 0x77
 * 	@author Knut Welzel
 */
__u8 bmp085_i2c_address = 0x77;


/**
 * Define default over sampling to "ultra high resolution"
 * \em low:     BMP085_OVERSAMPLING_LOW
 * \em default: BMP085_OVERSAMPLING_STANDARD
 * \em high:    BMP085_OVERSAMPLING_HIGH
 * \em ultra:   BMP085_OVERSAMPLING_ULTRA
 * \note The default value is BMP085_OVERSAMPLING_STANDARD
 * @author Knut Welzel
 */
unsigned char bmp085_oversampling = BMP085_OVERSAMPLING_STANDARD;


/**
 * Require the calibration.
 * \note Internal value, will be set to 1 (true) after first run.
 * @author Knut Welzel
 */
unsigned char bmb085_calibration_parameter = 0;



/** FUNKTIONS **/

#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "smbus.h"



/**
 * Open a connection a i2C connection
 * \note Internal function.
 * @author Knut Welzel
 * @return The i2c descriptor as an integer
 */
static inline int bmb085_i2c_open(__u8 addr) {

	int fd;
	char fn[10];

	sprintf(fn, "/dev/i2c-%d", bmp085_i2c_device);

	// Open port for reading and writing
	if((fd = open(fn, O_RDWR)) < 0) {

		printf("BMP085 error while open I2C device /dev/i2c-%d: %s\n", bmp085_i2c_device, strerror(errno));
		exit(1);
	}

	// Set the port options and set the address of the device
	if(ioctl(fd, I2C_SLAVE, addr) < 0) {

		printf("BMP085 error while open I2C slave 0x%x: %s\n", I2C_SLAVE, strerror(errno));
		close(fd);
		exit(1);
	}

	return fd;
}


/**
 * Setup the BMP085 sensor on first run.
 * \param i2c_device The number of the i2c device<br>
 * \em 0: /dev/i2c-0<br>
 * \em 1: /dev/i2c-1
 *
 * \param i2c_address The BMP085 i2c bus address<br>
 * default is 0x77
 *
 * \param oversampling Set over sampling mode.<br>
 * See BMP085 data sheet page 10 "overview of BMP085 over sampling modes":<br>
 * \li Low power:    BMP085_OVERSAMPLING_LOW<br>
 * \li Default:      BMP085_OVERSAMPLING_STANDARD<br>
 * \li Height:       BMP085_OVERSAMPLING_HIGH<br>
 * \li ultra Height: BMP085_OVERSAMPLING_ULTRA
 * \return No return value.
 *
 * @author Knut Welzel
 */
void bmp085_setup(__u8 i2c_device, __u8 i2c_address, unsigned char oversampling) {

	bmp085_i2c_device   = i2c_device;
	bmp085_i2c_address  = i2c_address;
	bmp085_oversampling = oversampling;
}


/**
 * get the calculation parameter
 * \return No return value.
 * \note Internal function
 * @author Knut Welzel
 */
static inline void bmb085_get_calibration_parameter() {

	// Open I2C line
	int fd = bmb085_i2c_open(bmp085_i2c_address);

	bmb085_calibration.ac1 = (short)bmp085_i2c_read_int(fd, 0xAA);
	bmb085_calibration.ac2 = (short)bmp085_i2c_read_int(fd, 0xAC);
	bmb085_calibration.ac3 = (short)bmp085_i2c_read_int(fd, 0xAE);
	bmb085_calibration.ac4 = (unsigned short)bmp085_i2c_read_int(fd, 0xB0);
	bmb085_calibration.ac5 = (unsigned short)bmp085_i2c_read_int(fd, 0xB2);
	bmb085_calibration.ac6 = (unsigned short)bmp085_i2c_read_int(fd, 0xB4);
	bmb085_calibration.b1  = (short)bmp085_i2c_read_int(fd, 0xB6);
	bmb085_calibration.b2  = (short)bmp085_i2c_read_int(fd, 0xB8);
	bmb085_calibration.mb  = (short)bmp085_i2c_read_int(fd, 0xBA);
	bmb085_calibration.mc  = (short)bmp085_i2c_read_int(fd, 0xBC);
	bmb085_calibration.md  = (short)bmp085_i2c_read_int(fd, 0xBE);

	bmb085_calibration_parameter = 1;

	// Close I2C line
	close(fd);
}


/**
 * Read two words from the BMP085 and supply it as a 16 bit integer
 * \param fd The I2C descriptor as an integer
 * \param reg_no The Register number as an inager
 * \return The entry of two registers as an intager
 * \note Internal function
 * @author Knut Welzel
 */
static inline __s32 bmp085_i2c_read_int(int fd, __u8 reg_no) {

	__s32 data;

	// Read 16bit register value
	data = i2c_smbus_read_word_data(fd, reg_no);

	// On error close connection
	if(data < 0) {

		perror("BMP085 error while read integer");
		close(fd);
		exit(1);
	}

	return ((data<<8) & 0xFF00) | ((data>>8) & 0xFF);
}


/**
 * Read the uncompensated temperature value
 * \return The raw value of the temperature as an unsigned integer
 * \note Internal function
 * @author Knut Welzel
 */
static inline unsigned int bmp085_get_ut() {

	unsigned int ut = 0;
	int fd = bmb085_i2c_open(bmp085_i2c_address);

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	bmp085_i2c_write_byte(fd,0xF4,0x2E);

	// Wait at least 4.5ms
	usleep(5000);

	// Read the two byte result from address 0xF6
	ut = bmp085_i2c_read_int(fd,0xF6);

	// Close the i2c file
	close(fd);

	return ut;
}


/**
 * Read the uncompensated pressure value
 * \return The raw value of the pressure as an unsigned integer
 * \note Internal function
 * @author Knut Welzel
 */
static inline unsigned int bmp085_get_up() {

	__u8 values[3];

	unsigned int up = 0;
	int fd = bmb085_i2c_open(bmp085_i2c_address);

	// Write 0x34+(BMP085_OVERSAMPLING_SETTING<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	bmp085_i2c_write_byte(fd,0xF4,0x34 + (bmp085_oversampling<<6));

	// Wait for conversion, delay time dependent on oversampling setting
	usleep((2 + (3<<bmp085_oversampling)) * 1000);

	// Read the three byte result from 0xF6
	// 0xF6 = MSB, 0xF7 = LSB and 0xF8 = XLSB
	if(i2c_smbus_read_i2c_block_data(fd, 0xF6, 3, values) < 0) {

		perror("Error while read I2C block:");
		close(fd);
		exit(1);
	}

	up = (((unsigned int) values[0] << 16)
	   | ((unsigned int) values[1] << 8)
	   | (unsigned int) values[2]) >> (8-bmp085_oversampling);

	// Close the i2c file
	close(fd);

	return up;
}


/**
 * Write a byte to the BMP085
 * \param fd The I2C descriptor as an intager
 * \param addr The register address of the BMP085
 * \param value The value to write into the register as a unsigned byte
 * \return No return value.
 * \note Internal function)
 * @author Knut Welzel
 */
static inline void bmp085_i2c_write_byte(int fd, __u8 addr, __u8 value) {

	if(i2c_smbus_write_byte_data(fd, addr, value) < 0) {

		perror("Error while read I2C byte:");
		close(fd);
		exit(1);
	}
}


/**
 * \brief Get the pressure.
 * \note Run 'bmp085_get_temperature()' before 'bmp085_get_pressure()' or use
 * 'bmp085_get_values' otherwise the pressure value is wrong.
 * \return Value will be returned as float in units of 0.01 mbar as pressure
 * @author Knut Welzel
 */
float bmp085_get_pressure(void) {

	if(bmb085_calibration_parameter == 0)
		bmp085_get_temperature();

	int x1, x2, x3, b3, b6, p;
	unsigned int b4, b7;

	b6 = bmb085_calibration.b5 - 4000;

	x1 = (bmb085_calibration.b2 * (b6 * b6)>>12)>>11;
	x2 = (bmb085_calibration.ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((int)bmb085_calibration.ac1) * 4 + x3)<<bmp085_oversampling) + 2)>>2;

	x1 = (bmb085_calibration.ac3 * b6)>>13;
	x2 = (bmb085_calibration.b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (bmb085_calibration.ac4 * (unsigned int)(x3 + 32768))>>15;

	b7 = ((unsigned int)(bmp085_get_up() - b3) * (50000>>bmp085_oversampling));
	if (b7 < 0x80000000)
		p = (b7<<1)/b4;
	else
		p = (b7/b4)<<1;

	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;

	return (float)p/100.0f;
}


/**
 * Get the temperature.
 * \return Value will be returned  as float in units of 0.1 deg C as temperature
 * @author Knut Welzel
 */
float bmp085_get_temperature(void) {

	float temperature;

	if(bmb085_calibration_parameter == 0)
		bmb085_get_calibration_parameter();

	int x1, x2;

	x1 = (((int)bmp085_get_ut() - (int)bmb085_calibration.ac6) * (int)bmb085_calibration.ac5) >> 15;
	x2 = ((int)bmb085_calibration.mc << 11)/(x1 + bmb085_calibration.md);
	bmb085_calibration.b5 = x1 + x2;

	temperature = (float)((bmb085_calibration.b5 + 8)>>4) / 10.0f;

	return temperature;
}


/**
 * Get altitude
 * \note On some soft float compilers, the calculation goes wrong
 * (e.g. Raspberry PI or Beagle needs hard float)
 * \return Value will be returned as float in units of meter as altitude
 * @author Knut Welzel
 */
float bmp085_get_altitude(float pressure) {

	return 44330.0f * (1.0f - powf(pressure/1013.25f, 0.1903f));
}


/**
 * Get temperature, pressure and altitude.
 * return Values will be returned as the struct bmp085_value
 * @author Knut Welzel
 */
struct bmp085_value bmp085_get_values(void) {

	struct bmp085_value value;

	if(bmb085_calibration_parameter == 0)
		bmb085_get_calibration_parameter();

	value.temperature = bmp085_get_temperature();
	value.pressure    = bmp085_get_pressure();
	value.altitude    = bmp085_get_altitude(value.pressure);

	return value;
}


#endif /* LIBBMP085_H_ */
