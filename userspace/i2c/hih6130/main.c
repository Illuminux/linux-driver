/**
 *  Copyright (C) 2013 Knut Welzel (knut@welzels.de)
 *
 *  main.c is a demo program for Honeywell Humidity and Temperature
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
 *  Dieses Programm ist Freie Software: Sie kšnnen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spŠteren
 *  veršffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  Dieses Programm wird in der Hoffnung, dass es nŸtzlich sein wird, aber
 *  OHNE JEDE GEWAHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 *  GewŠhrleistung der MARKTF€HIGKEIT oder EIGNUNG F†R EINEN BESTIMMTEN ZWECK.
 *  Siehe die GNU General Public License f†r weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include "../lib/libhih6130.h"

#define USAGE "Honeywell Digital Humidity/Temperature Sensors:\n" \
              "Usage: i2c-lib [OPTION]\n" \
	          "\n" \
	          "Options:\n" \
	          "  -v   Get all values\n" \
	          "  -t   Get Temperature\n" \
	          "  -rh  Get Humidity values\n" \



int main(int argc, char **argv) {

	char line[72];

	memset(&line[0], 0x2d, sizeof(line)-1);

	float temperature;
	float humidity;

	struct hih6130_value hih6130;

	if(argc == 2) {

		if(!strcmp(argv[1], "-v")) {

			hih6130 = hih6130_get_value();

			puts("\nHIH6310 values 'hih6130_get_value':");
			puts(line);
			printf("Temperature:\t %5.1fÂ°C\n", hih6130.temperature);
			printf("Humidity:\t %5.1fRh\n",    hih6130.humidity);
			printf("Status:\t\t %2d\n",        hih6130.status);
		}
		else if(!strcmp(argv[1], "-t")) {

			temperature = hih6130_get_temperature();

			puts("\nHIH6310 values 'hih6130_get_temperature':");
			puts(line);
			printf("Temperature:\t %5.1fï¿½C\n", temperature);
		}
		else if(!strcmp(argv[1], "-rh")) {

			humidity = hih6130_get_humidity();

			puts("\nHIH6310 values 'hih6130_get_humidity':");
			puts(line);
			printf("Humidity:\t %5.1fRh\n", humidity);
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
