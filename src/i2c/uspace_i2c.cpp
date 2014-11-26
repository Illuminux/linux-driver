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
