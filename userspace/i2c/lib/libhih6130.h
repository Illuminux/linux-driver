/**
 *  Copyright (C) 2013 Knut Welzel (knut@welzels.de)
 *
 *  libhih6130.c is a demo program for Honeywell Humidity and Temperature
 *  Sensor.
 *
 *  From user space you will need the SMBus level access helper functions
 *  written by Simon G. Vogl, Frodo Looijaard and Jean Delvare.
 *
 *  This program is free software: you can redistribute it and/or modify
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
 *  Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren
 *  veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
 *  OHNE JEDE GEWAHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 *  Siehe die GNU General Public License für weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 */

#ifndef HIH6130_H_
#define HIH6130_H_

#define HIH6130_STATUS_NORMAL  0
#define HIH6130_STATUS_STALE   1
#define HIH6130_STATUS_COMMAND 2

#define HIH6130_ALARM_HIGH_ON  0x18
#define HIH6130_ALARM_HIGH_OFF 0x19
#define HIH6130_ALARM_LOW_ON   0x1A
#define HIH6130_ALARM_LOW_OFF  0x1B

/** FUNCTION DEFINITONS **/

static inline int hih_i2c_open(__u8 addr);

struct hih6130_value hih6130_get_value(void);
float hih6130_get_humidity(void);
float hih6130_get_temperature(void);
unsigned char hih6130_get_status(void);

int hih6130_perform_command(unsigned char register_no);
float hih6130_get_command(unsigned char register_no);
void hih6130_set_command(unsigned char register_no, double humidity);

unsigned char hih6130_get_data(__u8 *data, int length);

float hih6130_calc_temperature(__u8 *data);
float hih6130_calc_humidity(__u8 *data);
void hih6130_calc_hex_humidity(float humidity, __u8 *data);

unsigned char hih6130_calc_status(int fd, int stausbit);


/** TYPE DEFINITIONS **/

struct hih6130_value {
	float temperature;
	float humidity;
	unsigned char status;
};



/** RUNTIME VARIABLES **/

/**
 * HIH6130 i2c device number
 * 	0: /dev/i2c-0
 * 	1: /dev/i2c-1
 */
__u8 hih6130_i2c_device = 1;


/**
 * HIH6130 i2c bus address
 * 	default is 0x27
 */
__u8 hih6130_i2c_address = 0x27;



/** FUNCTIONS **/

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "smbus.h"


//
// Open a connection a i2C connection - Returns a file id
//
static inline int hih_i2c_open(__u8 addr) {

	int fd;
	char fn[10];

	sprintf(fn, "/dev/i2c-%d", hih6130_i2c_device);

	// Open port for reading and writing
	if((fd = open(fn, O_RDWR)) < 0) {

		perror("Error while open I2C");
		exit(1);
	}

	// Set the port options and set the address of the device
	if(ioctl(fd, I2C_SLAVE, addr) < 0) {

		printf("Error while to I2C slave 0x%x: %s\n", I2C_SLAVE, strerror(errno));
		close(fd);
		exit(1);
	}

	return fd;
}


/**
 * Get temperature and humidity
 * Values will be returned as struct hih6130_value
 */
struct hih6130_value hih6130_get_value(void) {

	__u8 data[4];
	struct hih6130_value value;

	// Get the status data
	value.status = hih6130_get_data(data, sizeof(data));

	// Calculate humidity
	value.humidity = hih6130_calc_humidity(data);

	// Calculate temperature
	value.temperature = hih6130_calc_temperature(data);

	return value;
}


/**
 * Get humidity
 * Values will be returned as float in 0.1 Rh
 */
float hih6130_get_humidity() {

	__u8 data[2];

	// Get the first two byte of data register
	hih6130_get_data(data, sizeof(data));

	// Calculate the humidity
	return hih6130_calc_humidity(data);
}


/**
 * Get humidity
 * Values will be returned as float in 0.1�C
 * (This function also acquire the humidity data but will not return these)
 */
float hih6130_get_temperature() {

	struct hih6130_value value;

	// Get temperature and humidity data
	value = hih6130_get_value();

	// Return the temperature data
	return value.temperature;
}


/**
 * hih6130_calc_status(fd)
 */
unsigned char hih6130_get_status() {

	int fd;
	unsigned char status;

	// Open I2C connection
	fd = hih_i2c_open(hih6130_i2c_address);

	// Get sensor status
	status = hih6130_calc_status(fd, HIH6130_STATUS_NORMAL);

	close(fd);

	return status;
}


/**
 * Get temperature and humidity data (internal function)
 */
unsigned char hih6130_get_data(__u8 *data, int length) {

	int fd;
	unsigned char status;

	// Open I2C connection
	fd = hih_i2c_open(hih6130_i2c_address);

	// Get sensor status
	status = hih6130_calc_status(fd, HIH6130_STATUS_NORMAL);

	// Get values if status "Normal Operation" or "Stale Data"
	if(status <= 1) {

		// Read i2c values
		if(i2c_smbus_read_i2c_block_data(fd, 0x00, length, data) < 0) {

			perror("Error while read I2C block:");
			close(fd);
			exit(1);
		}
	}
	else {

		puts("HIH6130 is in Command Mode!");
		memset(&data[0], 0, sizeof(data));
	}

	// Close line
	close(fd);

	return status;
}


/**
 * Calculate the temperature (internal function)
 */
float hih6130_calc_temperature(__u8 *data) {

	int raw_temp;

	// Combine high and low byte, delete last two bits (14bit)
	raw_temp = (((int)data[2] << 8) + (int)data[3]) >> 2;

	// Calculate and return the temperature
	return ((float)raw_temp) / 16382.0f * 165.0f - 40.0f;
}


/**
 * Calculate the humidity (internal function)
 */
float hih6130_calc_humidity(__u8 *data) {

	int raw_humy;

	// Clear the status bits
	data[0] &= ~((0x01<<0x07) | (0x01<<0x06));

	// Combine high and low byte
	raw_humy = ((int)data[0] << 8) + (int)data[1];

	// Calculate and return the humidity
	return ((float)raw_humy) / 163.82f;
}


/**
 * calculate status (internal function)
 */
unsigned char hih6130_calc_status(fd, stausbit) {

	int i;
	__s32 status = 4;

	i2c_smbus_write_byte(fd, 0x00);

	for(i=0; i<10; i++) {

		status = (i2c_smbus_read_byte(fd)>>6);

		if(status == stausbit)
			break;
		else
			usleep(5000);
	}

	return (unsigned char)status;
}


/**
 *
 */
void hih6130_calc_hex_humidity(float humidity, __u8 *data) {
//void hih6130_calc_hex_humidity(double humidity) {

	long raw_humy;
//	__u8 data[2];

	raw_humy = (long)(humidity * 163.82);

	data[1] = raw_humy  & 0xff;
	data[0] = (raw_humy >> 8)  & 0xff;
}



float hih6130_get_command(unsigned char register_no) {

	int fd;
	__u8 data[2];

	fd = hih6130_perform_command(register_no);
	hih6130_calc_status(fd, 0);
	// Read data
	i2c_smbus_read_i2c_block_data(fd, 0x81, sizeof(data), data);

	close(fd);

	return hih6130_calc_humidity(data);
}



void hih6130_set_command(unsigned char register_no, double humidity) {

	int fd;
	__u8 data[2];

	// Convert humidity into hex array
	hih6130_calc_hex_humidity(humidity, data);

	fd = hih6130_perform_command(register_no);

	// Set write register
	register_no += 0x40;

	// Write level into register
	i2c_smbus_read_i2c_block_data(fd, register_no, sizeof(data), data);

	close(fd);
}



int hih6130_perform_command(unsigned char register_no) {

	int fd;

	__u8 data[2];

	memset(&data[0], 0, sizeof(data));

	// Open I2C connection
	fd = hih_i2c_open(hih6130_i2c_address);

	// Set to command mode
	i2c_smbus_write_i2c_block_data(fd, 0xA0, sizeof(data), data);

	// Set to EEPOROM Register
	i2c_smbus_write_i2c_block_data(fd, register_no, sizeof(data), data);

	return fd;
}


#endif /* HIH6130_H_ */
