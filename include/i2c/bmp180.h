/**
 * \page BMP180 BMP180 Digital Pressure Sensor
 *
 * This is a communication library for BOSCH BMP digital barometric Pressure
 * and Temperature Sensor.
 *
 * \li Class: This is an alias of bmp085
 * \li Header: bmp180.h
 *
 * The BMP180 is a high-precision, ultra-low power barometric pressure sensor
 * for use in advanced mobile applications.
 *
 * \b Example:
 *
 * \code{.cpp}
 *
 * #include <uspace.h>
 *
 * float temp;
 * float pres;
 * float alti;
 *
 * int  i2c_dev = 1; 		// /dev/i2c-1
 *
 * char i2c_adr = 0x77;		// I2C address of BMP180
 *
 * bmp180 bmp(i2c_dev, i2c_adr, bmp.oversampe_low);
 *
 * // Read temperature:
 *	bmp.get_temperature(temp);
 *
 *	// Read pressure:
 *	bmp.get_pressure(pres);
 *
 *	// Read temperature and pressure:
 *	bmp.get_values(pres, temp);
 *
 *	// Read temperature, pressure and altitude:
 *	bmp.get_values(pres, temp, alti);
 *
 * \endcode
 *
 * \copyright Copyright &copy; 2014 Knut Welzel (knut.welzel@googlemail.com)
 *
 */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 */
#ifndef BMP180_H_
#define BMP180_H_

#include <i2c/bmp085.h>

typedef bmp085 bmp180;

#endif /* BMP180_H_ */
