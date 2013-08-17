/**
 *  Copyright (C) 2013 Knut Welzel (knut@welzels.de)
 *
 *  lib24AAxxx.h is a communication library for Microchip I2C EEPROM
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
 *  OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 *  Siehe die GNU General Public License für weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIB24AAXX_H_
#define LIB24AAXX_H_

#define LIB24AAXX_ENDIAN 0x0001

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include "../lib/smbus.h"

static inline int e24AA_i2c_open(__u8 addr);
int e24AA_read(__u16 sector, char * buffer, size_t size);
int e24AA_write(__u16 sector, char * buffer, size_t size);
__u16 swap_u16(__u16 i);

/** TYPE DEFINITIONS **/


/** RUNTIME VARIABLES **/

/**
 * 24AAxxx i2c device number
 * 	0: /dev/i2c-0
 * 	1: /dev/i2c-1
 */
__u8 e24AA_i2c_device = 1;


/**
 * 24AAxxx i2c bus address
 * 	default is 0x27
 */
__u8 e24AA_i2c_address = 0x50;



/** FUNCTIONS **/


//
// Open a connection a i2C connection - Returns a file id
//
static inline int e24AA_i2c_open(__u8 addr) {

	int fd;
	char fn[10];

	sprintf(fn, "/dev/i2c-%d", e24AA_i2c_device);

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


int e24AA_read(__u16 sector, char * buffer, size_t size) {

	int fd;


	// Endian correction
	sector = swap_u16(sector);

	// Open i2c line
	fd = e24AA_i2c_open(e24AA_i2c_address);

	// Select sector
	write(fd, &sector, sizeof sector);

	// Wait until sector is selected
	usleep(5000);

	// Read from sector
	read(fd, buffer, size);

	// close i2c line
	close(fd);

	return 0;
}



int e24AA_write(__u16 sector, char * buffer, size_t size) {

	int fd;

	// Move buffer to the size of the sector
	memmove(buffer+sizeof sector, buffer, size+sizeof sector);

	// Endian correction
	sector = swap_u16(sector);

	// Insert sector adders at the beginning of buffer
	memcpy(buffer, &sector, sizeof sector);

	// Open i2c line
	fd = e24AA_i2c_open(0x50);

	// write data to sector arrdess
	write(fd, buffer, size + sizeof sector);

	// close i2c line
	close(fd);

	// wait
	usleep(5000);

	return 0;
}


__u16 swap_u16(__u16 uint16) {

    __u8 hbyte, lbyte;
    int num = 1;

    // Little-Endian
    if(*(char *)&num == 1) {
		hbyte = uint16 & 255;
		lbyte = (uint16 >> 8) & 255;

		return (hbyte << 8) + lbyte;
    }

    // Big-Endian
    return uint16;
}


#endif /* LIB24AAXX_H_ */
