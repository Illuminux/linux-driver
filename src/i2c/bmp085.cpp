
#include <i2c/bmp085.h>


//
// Public Methods
//


//
// Constructor, setup the BMP085
bmp085::bmp085(int i2c_dev, __u8 i2c_addr, __u8 osamp) {

	int fd;

	// Format the device name for class global
	sprintf(device, "/dev/i2c-%d", i2c_dev);

	// Set i2c address for class global
	address    = i2c_addr;

	// Set oversampling mode for class global
	oversample = osamp;

	// open i2c communication
	if(i2c_open(fd) == 0) {

		// Read calibration parameter from BMP EEPROM and set for class global
		calibration.ac1 = (short)i2c_read_int(fd, 0xAA);
		calibration.ac2 = (short)i2c_read_int(fd, 0xAC);
		calibration.ac3 = (short)i2c_read_int(fd, 0xAE);
		calibration.ac4 = (unsigned short)i2c_read_int(fd, 0xB0);
		calibration.ac5 = (unsigned short)i2c_read_int(fd, 0xB2);
		calibration.ac6 = (unsigned short)i2c_read_int(fd, 0xB4);
		calibration.b1  = (short)i2c_read_int(fd, 0xB6);
		calibration.b2  = (short)i2c_read_int(fd, 0xB8);
		calibration.mb  = (short)i2c_read_int(fd, 0xBA);
		calibration.mc  = (short)i2c_read_int(fd, 0xBC);
		calibration.md  = (short)i2c_read_int(fd, 0xBE);

		// Close i2c communication
		close(fd);
	}
}


//
// Get the temperature in units of 0.1 deg C
int bmp085::get_temperature(float& temperature) {

	int fd;

	int x1, x2;

	x1 = (((int)get_ut(fd) - (int)calibration.ac6) * (int)calibration.ac5) >> 15;
	x2 = ((int)calibration.mc << 11)/(x1 + calibration.md);

	calibration.b5 = x1 + x2;

	temperature = (float)((calibration.b5 + 8)>>4) / 10.0f;

	return 0;
}


// Get the pressure in units of 0.01 mbar.
int bmp085::get_pressure(float& pressure) {

	float temperature;

	return get_values(pressure, temperature);
}


// Get the pressure in units of 0.01 mbar, temperature in units of 0.1 deg C and
// altitude in meters
int bmp085::get_values(float &pressure, float &temperature, float &altitude) {

	get_values(pressure, temperature);

	altitude = 44330.0f * (1.0f - powf(pressure/1013.25f, 0.1903f));

	return 0;
}


// Get the pressure in units of 0.01 mbar and temperature in units of 0.1 deg C
int bmp085::get_values(float &pressure, float &temperature) {

	int fd;
	int x1, x2, x3, b3, b6, p;
	unsigned int b4, b7;


	get_temperature(temperature);

	b6 = calibration.b5 - 4000;
	x1 = (calibration.b2 * (b6 * b6)>>12)>>11;
	x2 = (calibration.ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((int)calibration.ac1) * 4 + x3) << oversample) + 2)>>2;

	x1 = (calibration.ac3 * b6)>>13;
	x2 = (calibration.b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (calibration.ac4 * (unsigned int)(x3 + 32768))>>15;
	b7 = ((unsigned int)(get_up(fd) - b3) * (50000>>oversample));

	if (b7 < 0x80000000)
		p = (b7<<1)/b4;
	else
		p = (b7/b4)<<1;

	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;

	pressure = (float)p/100.0f;

	return 0;
}



//
// Private Methods
//


//
// Read the uncompensated temperature value
unsigned int bmp085::get_ut(int &fd) {

	unsigned int ut = 0;
	i2c_open(fd);

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	i2c_smbus_write_byte_data(fd,0xF4,0x2E);

	// Wait at least 4.5ms
	usleep(5000);

	// Read the two byte result from address 0xF6
	ut = i2c_read_int(fd, 0xF6);

	// Close the i2c file
	close(fd);

	return ut;
}


//
// Read the uncompensated pressure value
unsigned int bmp085::get_up(int &fd) {

	__u8 values[3];

	unsigned int up = 0;

	i2c_open(fd);

	// Write 0x34+(BMP085_OVERSAMPLING_SETTING<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	i2c_smbus_write_byte_data(fd, 0xF4, (0x34 + (oversample<<6)));

	// Wait for conversion, delay time dependent on oversampling setting
	usleep((2 + (3<<oversample)) * 1000);

	// Read the three byte result from 0xF6
	// 0xF6 = MSB, 0xF7 = LSB and 0xF8 = XLSB
	if(i2c_smbus_read_i2c_block_data(fd, 0xF6, 3, values) < 0) {

		perror("Error while read I2C block:");
		close(fd);
		return 0;
	}

	up = (((unsigned int) values[0]  <<  16)
	   |  ((unsigned int) values[1]  <<  8)
	   |   (unsigned int) values[2]) >> (8-oversample);

	// Close the i2c file
	close(fd);

	return up;
}


//
// Open a connection a i2C connection
int bmp085::i2c_open(int &fd) {


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



//
// Read two words from the BMP085 and supply it as a 16 bit integer
//
__s32 bmp085::i2c_read_int(int &fd, __u8 reg_no) {

	__s32 data;

	// Read 16bit register value
	data = i2c_smbus_read_word_data(fd, reg_no);

	// On error close connection
	if(data < 0) {

		perror("Error while read integer from i2c");
		return -1;
	}

	return ((data<<8) & 0xFF00) | ((data>>8) & 0xFF);
}
