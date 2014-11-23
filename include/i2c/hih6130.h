/**
 *  Copyright (C) 2014 Knut Welzel (knut.welzel@googlemail.com)
 *
 *  This is a communication library for Honeywell Humidity and
 *  Temperature Sensor HIH6130/6131 Series.
 */
/*
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

#include <cstdio>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>			// Use libi2c-dev

class hih6130 {

	/**
	 * @brief   HIH6130 i2c device number
	 * @em 0:   /dev/i2c-0
	 * @em 1:   /dev/i2c-1
	 * @author  Knut Welzel
	 */
	char device[12];


	/**
	 * @brief   HIH6130 i2c bus address.
	 * @note    The default address is 0x27
	 * @author Knut Welzel
	 */
	__u8 address;

public:

	/**
	 * @brief   HIH6130 constructor
	 * @param   i2c_dev  I2C device number
	 * @param   i2c_addr HIH6130 I2C address
	 * @author  Knut Welzel
	 */
	hih6130(int i2c_dev, __u8 i2c_addr);

	/**
	 * @brief   Get humidity
	 * @param   humidity Humidity in %RH as pointer
	 * @return  Sensor staus
	 */
	int get_humidity(float &humidity);


	/**
	 * @brief   Get temperature
	 * @param   temperature temperature in °C as pointer
	 * @return  Sensor staus
	 */
	int get_temperature(float &temperature);


	/**
	 * @brief   Get humidity and temperature
	 * @param   humidity Humidity in %RH as pointer
	 * @param   temperature Temperature in °C as pointer
	 * @return  Sensor staus
	 */
	int get_values(float &humidity, float &temperature);


	/**
	 *
	 */
	int get_status(void);


private:

	/**
	 * @brief   Diagnostic States Indicated by Status Bits.
	 */
	enum {
		STATUS_NORMAL,				///< normal operation
		STATUS_STALE,				///< data that has already been fetched
		STATUS_COMMAND,				///< device in Command Mode
		STATUS_DIAGNOSTIC			///< diagnostic condition
	};


	/**
	 * @brief  Get raw temperature and humidity data
	 * @param  data    Raw data array
	 * @param  length  Length of raw data array
	 * @return Sensor staus
	 */
	int get_data(__u8 *data, int length);


	/**
	 * @brief  Calculate the humidity
	 * @param  humidity Humidity in %RH as pointer
	 */
	void calc_humidity(float &humidity, __u8 *data);


	/**
	 * @brief  Calculate the temperature
	 * @param  temperature Temperature in °C as pointer
	 */
	void calc_temperature(float &temperature, __u8 *data);


	/**
	 * @brief  Calculate status
	 * @param  fd The i2c descriptor as an integer
	 * @param  stausbit Diagnostic States
	 * @return Sensor staus
	 */
	int calc_status(int &fd, int stausbit);


	/**
	 * @brief  Calculate humidity as hex array
	 * @param  humidity Humidity in %RH
	 * @param  data Empty array
	 */
	void calc_hex_humidity(float humidity, __u8 *data);


	/**
	 * @brief  Open a connection a i2C connection
	 * @param  fd The i2c descriptor as an integer
	 * @return Error Code
	 * @author Knut Welzel
	 */
	int i2c_open(int &fd);
};


#endif /* HIH6130_H_ */
