/*
 * hih6130.cpp
 *
 *  Created on: 22.11.2014
 *      Author: knut
 */

#include <i2c/hih6130.h>


//
// Constructor, setup the hih6130
hih6130::hih6130(int i2c_dev, __u8 i2c_addr) {

	// Format the device name for class global
	sprintf(device, "/dev/i2c-%d", i2c_dev);

	// Set i2c address for class global
	address    = i2c_addr;
}


//
// Get humidity
int hih6130::get_humidity(float &humidity) {

	int status;
	__u8 data[2];

	// Get the first two byte of data register
	status = get_data(data, sizeof(data));

	// calculate humidity
	calc_humidity(humidity, data);

	return status;
}


// Get temperature
int hih6130::get_temperature(float &temperature) {

	int status;
	float humidity;

	status = get_values(humidity, temperature);

	return status;
}


// Get temperature and humidity
int hih6130::get_values(float &humidity, float &temperature) {

	int status;
	__u8 data[4];

	// Get the status data
	status = get_data(data, sizeof(data));

	// Calculate humidity
	calc_humidity(humidity, data);

	// Calculate temperature
	calc_temperature(temperature, data);

	return status;
}


//
//
int hih6130::get_status() {

	int fd;
	unsigned char status;

	// Get sensor status
	status = calc_status(fd, STATUS_NORMAL);

	return status;
}


//
// Get raw data
int hih6130::get_data(__u8 *data, int length) {

	int fd;
	unsigned char status;

	// Open I2C connection
	if(i2c_open(fd) == 0) {

		// Get sensor status
		status = calc_status(fd, STATUS_NORMAL);

		// Get values if status "Normal Operation" or "Stale Data"
		if(status == STATUS_NORMAL || status == STATUS_STALE) {

			// Read i2c values
			if(i2c_smbus_read_i2c_block_data(fd, 0x00, length, data) < 0) {

				perror("Error while read I2C block:");
				close(fd);
				return 0;
			}
		}
		else {

			puts("HIH6130 is in Command Mode!");
			memset(&data[0], 0, sizeof(data)*sizeof(__u8));
		}
	}

	// Close i2c communication
	close(fd);

	return status;
}



// Calculate humidity
void hih6130::calc_humidity(float &humidity, __u8 *data) {

	int raw_humy;

	// Clear the status bits
	data[0] &= ~((0x01<<0x07) | (0x01<<0x06));

	// Combine high and low byte
	raw_humy = ((int)data[0] << 8) + (int)data[1];

	// Calculate and return the humidity
	humidity = ((float)raw_humy) / 163.82f;

	return;
 }


// Calculate temperature
void hih6130::calc_temperature(float &temperature, __u8 *data) {

	int raw_temp;

	// Combine high and low byte, delete last two bits (14bit)
	raw_temp = (((int)data[2] << 8) + (int)data[3]) >> 2;

	// Calculate and return the temperature
	temperature = ((float)raw_temp) / 16382.0f * 165.0f - 40.0f;

	return;
}


// calculate status
int hih6130::calc_status(int &fd, int stausbit) {

	int i;
	__s32 status = 4;

	i2c_smbus_write_byte(fd, 0x00);

	for(i=0; i<10; i++) {

		status = (i2c_smbus_read_byte(fd)>>6);

		if(status == stausbit)
			break;
		else
			usleep(5000);
	}

	return status;
}


// convert humidity in hex array
void hih6130::calc_hex_humidity(float humidity, __u8 *data) {

	long raw_humy;

	raw_humy = (long)(humidity * 163.82);

	data[1] = raw_humy  & 0xff;
	data[0] = (raw_humy >> 8)  & 0xff;

	return;
}

//
// Open a connection a i2C connection
int hih6130::i2c_open(int &fd) {


	// Open port for reading and writing
	if((fd = open(device, O_RDWR)) < 0) {
		fprintf(
				stderr,
				"Error while open I2C device %s: %s\n",
				device,
				strerror(errno));

		return errno;
	}

	// Set the port options and set the address of the device
	if(ioctl(fd, I2C_SLAVE, address) < 0) {

		fprintf(
				stderr,
				"Error while open I2C slave 0x%x: %s\n",
				I2C_SLAVE,
				strerror(errno));

		close(fd);
		return errno;
	}

	return 0;
}
