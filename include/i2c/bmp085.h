/**
 * \page BMP085 BMP085 Digital Pressure Sensor
 *
 * This is a communication library for BOSCH BMP digital barometric Pressure
 * and Temperature Sensor.
 *
 * \li Class: bmp085
 * \li Header: bmp085.h
 *
 * The BMP085 is a high-precision, ultra-low power barometric pressure sensor
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
 * int  i2c_dev = 1; 			// /dev/i2c-1
 *
 * char i2c_adr = 0x77;		// I2C address of BMP085
 *
 * bmp085 bmp(i2c_dev, i2c_adr, bmp.oversampe_low);
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

#ifndef BMP085_H_
#define BMP085_H_

#include <i2c/uspace_i2c.h>
#include <math.h>

/**
 * bmp085
 */
class bmp085: public uspace_i2c {

public:

	/**
	 * @brief  Define default over sampling to "ultra high resolution"
	 * @em low:     BMP085_OVERSAMPLING_LOW
	 * @em default: BMP085_OVERSAMPLING_STANDARD
	 * @em high:    BMP085_OVERSAMPLING_HIGH
	 * @em ultra:   BMP085_OVERSAMPLING_ULTRA
	 *
	 * @note   The default value is BMP085_OVERSAMPLING_STANDARD
	 * @author Knut Welzel
	 */
	unsigned char oversample;

	/**
	 * @brief  Constructor, setups the BMP085
	 * @param  i2c_dev The number of the i2c device <br>
	 * @em 0:  /dev/i2c-0 <br>
	 * @em 1:  /dev/i2c-1
	 * @param  i2c_addr The BMP085 i2c bus address (default is 0x77)
	 * @param  osamp Set over sampling mode.<br>
	 *        See BMP085 data sheet page 10 "overview of BMP085 over sampling
	 *        modes":<br>
	 * @li Low power:  oversampe_low<br>
	 * @li Default:    oversampe_standard<br>
	 * @li High:       oversampe_ligh<br>
	 * @li Ultra High: oversampe_ultra
	 *
	 * @return No return value.
	 *
	 * @author Knut Welzel
	 */
	bmp085(int i2c_dev, __u8 i2c_addr, __u8 osamp);


	/**
	 * @brief  BMP085 Over sampling Modes
	 */
	enum oversampe {
		oversampe_low,					///< Low over sampling mode
		oversampe_standard,				///< Default over sampling mode
		oversampe_high,					///< High over sampling mode
		oversampe_ultra					///< Ultra high over sampling mode
	};



	/**
	 * @brief  Get the temperature in units of 0.1 deg C.
	 * @param  temperature Pointer to temperature
	 * @return Error code
	 * @author Knut Welzel
	 */
	int get_temperature(float& temperature);



	/**
	 * @brief  Get the pressure in units of 0.01 mbar.
	 * @param  pressure Pointer to pressure
	 * @return Error code
	 * @author Knut Welzel
	 */
	int get_pressure(float& pressure);



	/**
	 * @brief  Get the pressure in units of 0.01 mbar and the temperature in
	 *        units of 0.1 deg C.
	 * @param  pressure Pointer to pressure
	 * @param  temperature Pointer to temperature
	 * @return Error code
	 * @author Knut Welzel
	 */
	int get_values(float& pressure, float& temperature);



	/**
	 * @brief  Get the pressure in units of 0.01 mbar, the temperature in
	 *        units of 0.1 deg C and altitude in meters.
	 * @param  pressure    Pointer to pressure
	 * @param  temperature Pointer to temperature
	 * @param  altitude    Pointer to altitude
	 * @return Error code
	 * @author Knut Welzel
	 */
	int get_values(float& pressure, float& temperature, float &altitude);


private:

	/**
	 * @brief  Calibration values. These values are stored in the EEPROM of the
	 *        BMP085 sensor.
	 * @author Knut Welzel
	 */
	struct calibration {
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
	struct calibration calibration;


	/**
	 * @brief  Read the uncompensated temperature value
	 * @param  fd The i2c descriptor as an integer
	 * @return The raw value of the temperature as an unsigned integer
	 * @author Knut Welzel
	 */
	unsigned int get_ut(int &fd);


	/**
	 * @brief  Read the uncompensated pressure value
	 * @param  fd The i2c descriptor as an integer
	 * @return The raw value of the pressure as an unsigned integer
	 * @author Knut Welzel
	 */
	unsigned int get_up(int &fd);


	/**
	 * @brief  Open a connection a i2C connection
	 * @param  fd The i2c descriptor as an integer
	 * @return Error Code
	 * @author Knut Welzel
	 */
//	int i2c_open(int &fd);


	/**
	 * @brief  Read two words from the BMP085 and supply it as a 16 bit integer
	 * \param  fd The I2C descriptor as an integer
	 * \param  reg_no The Register number as an inager
	 * \return The entry of two registers as an intager
	 * @author Knut Welzel
	 */
	__s32 i2c_read_int(int &fd, __u8 reg_no);

};


#endif /* BMP085_H_ */
