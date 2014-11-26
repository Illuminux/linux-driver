/*
 * uspace_i2c.cpp
 *
 *  Created on: 26.11.2014
 *      Author: knut
 */


#include <i2c/uspace_i2c.h>


//
// Open a connection a i2C connection
int uspace_i2c::i2c_open(int &fd) {


	// Open port for reading and writing
	if((fd = open(device, O_RDWR)) < 0) {
		fprintf(
				stderr,
				"Error while open I2C device %s: %s\n",
				device,
				strerror(errno));

		return errno;
	}

	// Set the port options and set the address of the device
	if(ioctl(fd, I2C_SLAVE, address) < 0) {

		fprintf(
				stderr,
				"Error while open I2C slave 0x%x: %s\n",
				I2C_SLAVE,
				strerror(errno));

		close(fd);
		return errno;
	}

	return 0;
}


//
// Read two words from the BMP085 and supply it as a 16 bit integer
//
__s32 uspace_i2c::i2c_read_int(int &fd, __u8 reg_no) {

	__s32 data;

	// Read 16bit register value
	data = i2c_smbus_read_word_data(fd, reg_no);

	// On error close connection
	if(data < 0) {

		perror("Error while read integer from i2c");
		return -1;
	}

	return ((data<<8) & 0xFF00) | ((data>>8) & 0xFF);
}
