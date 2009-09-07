//
// C++ Implementation: I2CDevice
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
using namespace std;
#include <QTimer>
#include "i2cdevice.h"
#include "i2c-dev.h"

I2CDevice::I2CDevice(int addressOfDevice)
{
	state = ok;

	address = addressOfDevice;

#ifdef __arm__	
	/* Open adapter i2c */
	if ((file = open("/dev/i2c-0",O_RDWR)) < 0) {
		qDebug("Can't open i2c device");
		state = busError;
	} else {
		/* open sensor */
		if (ioctl(file,I2C_SLAVE,address) < 0) {
			/* ERROR HANDLING; you can check errno to see what went wrong */
			qDebug("Can't open address: %d", address);
			state = deviceError;
		}
		if (ioctl(file,I2C_TIMEOUT,2) < 0) {
			/* ERROR HANDLING; you can check errno to see what went wrong */
			qDebug("Can't set timeout");
			state = deviceError;
		}
	}
#endif

}


I2CDevice::~I2CDevice()
{
#ifdef __arm__
	close(file);
#endif
}

void I2CDevice::writeI2C(char* bufor, int length)
{
#ifdef __arm__
	if (state == ok || state == writeError || state == readError || state == deviceError ){
		/* open sensor */
		if (ioctl(file,I2C_SLAVE,address) < 0) {
			/* ERROR HANDLING; you can check errno to see what went wrong */
			qDebug("Can't open address: %d", address);
			state = deviceError;
		}
	}

	if (state == ok || state == writeError || state == readError ){
		if ( write(file,bufor,length) != length) {
			/* ERROR HANDLING: i2c transaction failed */
			qDebug("Can't write to device");
			state = writeError;
		}
	}
#endif
}

void I2CDevice::readI2C(char* bufor, int length)
{
#ifdef __arm__
	if (state == ok || state == writeError || state == readError || state == deviceError){
		/* open sensor */
		if (ioctl(file,I2C_SLAVE,address) < 0) {
			/* ERROR HANDLING; you can check errno to see what went wrong */
			qDebug("Can't open address: %d", address);
			state = deviceError;
		}
	}

	if (state == ok || state == writeError || state == readError ){
		if ( read(file,bufor,length) != length) {
			/* ERROR HANDLING: i2c transaction failed */
			qDebug("Can't read from device");
			state = readError;
		}
	}
#else
	for (int i=0;i<length;i++){
		bufor[i] = i;
	}
#endif
}