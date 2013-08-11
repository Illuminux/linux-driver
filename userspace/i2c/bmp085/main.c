/**
 *  Copyright (C) 2013 Knut Welzel (knut.welzels@googlemail.com)
 *
 *  main.c is a demo program for BOSCH Barometric Pressure
 *  and Temperature Sensor.
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
 *
 *  Compiling Options:
 *   -lm
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/i2c-dev.h>

#include "../lib/libbmp085.h"


#define USAGE "BOSCH Digital Pressure Sensor\n" \
			  "Usage: i2c-lib [OPTION]\n" \
	          "\n" \
	          "Options:\n" \
	          "-v   Get all values\n" \
	          "-t   Get temperature\n" \
	          "-p   Get pressure\n" \
	          "-a   Get altitude\n"


int main(int argc, char **argv) {

	char line[72];

	memset(&line[0], 0x2d, sizeof(line)-1);

	float altitude;
	float temperature;
	float pressure;

	struct bmp085_value bmp085;

	if(argc > 1) {


		bmp085_setup(1, 0x77, BMP085_OVERSAMPLING_LOW);


		if(!strcmp(argv[1], "-v")) {

			bmp085 = bmp085_get_values();
			puts("\nBMP085 values 'bmp085_get_values()':");
			puts(line);
			printf("Temperature:\t %6.1f°C\n", bmp085.temperature);
			printf("Pressure:\t %6.1fmbar\n",  bmp085.pressure);
			printf("Altitude:\t %.1fm\n",  bmp085.altitude);
		}
		else if(!strcmp(argv[1], "-p")) {
			pressure = bmp085_get_pressure();
			puts("\nBMP085 pressure 'bmp085_get_pressure()':");
			puts(line);
			printf("Pressure:\t %5.1fmbar\n", pressure);
		}
		else if(!strcmp(argv[1], "-t")) {

			temperature = bmp085_get_temperature();
			puts("\nBMP085 temperature 'bmp085_get_temperature()':");
			puts(line);
			printf("Temperature:\t %5.1f°C\n", temperature);
		}
		else if(!strcmp(argv[1], "-a")) {

			altitude = bmp085_get_altitude(bmp085_get_pressure());
			puts("\nBMP085 altitude 'bmp085_get_altitude(float pressure)':");
			puts(line);
			printf("Altitude:\t %5.1fm\n", altitude);
		}
		else if((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {

			puts(USAGE);
		}
		else {
			puts("Error: No option selected!");
			puts(USAGE);
		}
	}
	else {
		puts("Error: No option selected!");
		puts(USAGE);
	}

	return 0;
}
