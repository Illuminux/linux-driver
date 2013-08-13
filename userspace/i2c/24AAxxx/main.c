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
 *  OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 *  Siehe die GNU General Public License für weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "../lib/lib24AAxxx.h"

int main(int argc, char **argv) {

	unsigned short adresse;

	adresse = 1;

	int fd;

	char sector[2];

	char rbyte[2];
	char wbyte[1];


	wbyte[0] = 9;



	memcpy(sector, &adresse, sizeof adresse);

	e24AA_read(wbyte, adresse);

/*
	memcpy(wbyte, &adresse, sizeof adresse);

	wbyte[2] = 0x06;

	// Byte Write
	fd = e24AA_i2c_open(0x50);

	write(fd, wbyte, sizeof wbyte);
	close(fd);
*/
	usleep(5000);

	// Byte Read
	fd = e24AA_i2c_open(0x50);

	write(fd, sector, sizeof sector);
	usleep(5000);
	read(fd, rbyte, 1);
	close(fd);

	printf("Text: %x\n", rbyte[0]);

	return 0;
}
