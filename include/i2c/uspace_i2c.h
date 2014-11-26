/*
 * i2c.h
 *
 *  Created on: 26.11.2014
 *      Author: knut
 */

#ifndef I2C_H_
#define I2C_H_

#include <cstdio>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>			// Use libi2c-dev

class uspace_i2c {

public:

	/**
	 * @brief The i2c device number
	 * @em 0:   /dev/i2c-0
	 * @em 1:   /dev/i2c-1
	 */
	char device[12];


	/**
	 * @brief The i2c bus address do the device.
	 */
	__u8 address;


	/**
	 * @brief  Open a connection a i2C connection
	 * @param  fd The i2c descriptor as an integer
	 * @return Error Code
	 * @author Knut Welzel
	 */
	int i2c_open(int &fd);


	/**
	 * @brief  Read two words from and supply it as a 16 bit integer
	 * \param  fd The I2C descriptor as an integer
	 * \param  reg_no The Register number as an inager
	 * \return The entry of two registers as an intager
	 * @author Knut Welzel
	 */
	__s32 i2c_read_int(int &fd, __u8 reg_no);

private:
};


#endif /* I2C_H_ */
