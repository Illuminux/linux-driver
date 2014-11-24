/*
 * main.cpp
 *
 *  Created on: 21.11.2014
 *      Author: knut
 */

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <uspace.h>

void print_usage() {

	puts("BOSCH Digital Pressure Sensor\n"
			"Usage: i2c-lib [OPTION]\n"
			"\n"
			"Options:\n"
			"  -v   Get all values\n"
			"  -t   Get temperature\n"
			"  -p   Get pressure\n"
			"  -a   Get altitude\n"
			"  -s   Scan in 1 second intervall\n");
}

int main(int argc, char **argv) {

	float temp;
	float pres;
	float alti;

	bmp085 bmp(1, 0x77, bmp.oversampe_low);

	if (argc > 1) {

		if (!strcmp(argv[1], "-v")) {

			bmp.get_values(pres, temp);
			printf(
					"Temperature:\t %3.2f°C\n"
					"Pressure:\t %6.1fmbar\n",
					temp,
					pres);
		}
		else if(!strcmp(argv[1], "-p")) {

			bmp.get_pressure(pres);
			printf("Pressure:\t %5.1fmbar\n", pres);
		}
		else if(!strcmp(argv[1], "-t")) {

			bmp.get_temperature(temp);
			printf("Temperature:\t %5.1f°C\n", temp);
		}
		else if(!strcmp(argv[1], "-s")) {

			system("clear");

			puts("┌─────────────┬─────────────┬─────────────┬─────────────┐");
			puts("│ Time        │ Temperature | Pressure    |    Altitude │");
			puts("├─────────────┼─────────────┼─────────────┼─────────────┤");
			puts("│             │             |             |             │");
			puts("└─────────────┴─────────────┴─────────────┴─────────────┘");
			printf("\033[F\033[F");
			fflush(stdout);

			while(1) {

				// current date/time based on current system
				time_t now = time(0);
				tm *ltm = localtime(&now);

				bmp.get_values(pres, temp, alti);

				printf("│    %02d:%02d:%02d | %9.1f°C | %7.1fmbar |  %9.1fm |\r",
						ltm->tm_hour,
						ltm->tm_min,
						ltm->tm_sec,
						temp,
						pres,
						alti);
				fflush(stdout);
				sleep(1);
			}
		}
		else {
			print_usage();
		}
	}
	else {

		bmp.get_values(pres, temp, alti);
		printf(
				"Temperature:\t %3.2f°C\n"
				"Pressure:\t %6.1fmbar\n"
				"Altitude:\t %.1fm\n",
				temp,
				pres,
				alti);
	}

	return 0;
}
