/*
 * main.cpp
 *
 *  Created on: 22.11.2014
 *      Author: knut
 */

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <uspace.h>

void print_usage() {

	puts("Honeywell Digital Humidity/Temperature Sensors\n"
			"Usage: hih6130 [OPTION]\n"
			"\n"
			"Options:\n"
			"  -v   Get all values\n"
			"  -t   Get temperature\n"
			"  -rh  Get humidity\n"
			"  -s   Scan in 1 second intervall\n");
}


int main(int argc, char **argv) {

	float humi;
	float temp;

	hih6130 hih(1, 0x27);

	if (argc > 1) {

		if (!strcmp(argv[1], "-v")) {

			hih.get_values(humi, temp);
			printf(
					"Temperature:\t %3.2f°C\n"
					"Humidity:\t %.1fm\n",
						temp,
						humi);

		}
		else if(!strcmp(argv[1], "-t")) {

			hih.get_temperature(temp);
			printf("Temperature:\t %3.1f°C\n", temp);
		}
		else if(!strcmp(argv[1], "-rh")) {

			hih.get_humidity(humi);
			printf("Humidity: \t %3.1f%%RH\n", humi);
		}
		else if(!strcmp(argv[1], "-s")) {

			system("clear");

			puts("┌─────────────┬─────────────┬─────────────┐\n");
			puts("│ Time        │ Temperature | Humidity    │\n");
			puts("├─────────────┼─────────────┼─────────────┤\n");
			puts("│             │             |             │\n");
			puts("└─────────────┴─────────────┴─────────────┘\n");
			printf("\033[F\033[F");
			fflush(stdout);

			while(1) {

				// current date/time based on current system
				time_t now = time(0);
				tm *ltm = localtime(&now);

				hih.get_values(humi, temp);

				printf("\r│    %02d:%02d:%02d | %9.1f°C | %8.1f%%RH |",
						ltm->tm_hour,
						ltm->tm_min,
						ltm->tm_sec,
						temp,
						humi);

				fflush(stdout);
				sleep(1);
			}

		}
		else {
			print_usage();
		}
	}
	else {

		hih.get_values(humi, temp);
		printf(
				"Temperature:\t %3.1f°C\n"
				"Humidity:\t %.1fm\n",
					temp,
					humi);
	}

	return 0;
}
