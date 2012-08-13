//
// C++ Implementation: ADCDevice
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "adcdevice.h"
#include "i2cdevice.h"
#include <unistd.h>
#include <QTimer>
#include <cstdlib>

ADCDevice::ADCDevice()
{
	i2CDevice = new I2CDevice(75); //address I2C of ADC
	interval = 0;
}

int ADCDevice::singleConversion(int channel)
{
	char buf[8];
	int result;

	if(interval != 0) interval->stop();
	
	switch(channel){
		case 1: buf[0] = AIN03; break;
		case 2: buf[0] = AIN13; break;
		case 3: buf[0] = AIN23; break;
	}
	buf[0] = buf[0] | SPS15 | GAIN1;

	i2CDevice->writeI2C(buf, 1);

	usleep(200000);

	i2CDevice->readI2C(buf, 2);

#ifdef __arm__
	result = ( (signed char)buf[0]*256+buf[1] );
#else
	result = 32500;
#endif

	return result;
}

void ADCDevice::startConversions(int channels, int period)
{
	char buf[8];

	buf[0] = AIN23 | SPS15 | GAIN1;

	i2CDevice->writeI2C(buf, 1);

	numberOfCannels = channels;
	currentChannel = 3;

	if(interval == 0)
	{
		interval = new QTimer(this);
		connect(interval, SIGNAL(timeout()), this, SLOT(intervalElapsed()));
	}
	interval->start(period);
}

void ADCDevice::stopConversion()
{
	if(interval != 0) interval->stop();
}

void ADCDevice::intervalElapsed()
{
	int result;
	char buf[8];
	i2CDevice->readI2C(buf, 2);
	result = ( (signed char)buf[0]*256+buf[1] );
	
	switch(currentChannel)
	{
	    case 3:
#ifdef __arm__
		emit(channel3(result));
#else
		emit(channel3(rand()%16001));
#endif
		if(numberOfCannels>1) {
			buf[0] = AIN13;
			currentChannel = 2;
		} else {
			buf[0] = AIN23;
			currentChannel = 3;
		}
	    break;
	    case 2:
#ifdef __arm__
		emit(channel2(result));
#else
		emit(channel2(30000));//  10000 is around 140 C
#endif
		if(numberOfCannels>2) {
			buf[0] = AIN03;
			currentChannel = 1;
		} else {
			buf[0] = AIN23;
			currentChannel = 3;
		}
	    break;
	    case 1:
#ifdef __arm__
		emit(channel1(result));
#else
		emit(channel1(5000));
#endif
		buf[0] = AIN23;
		currentChannel = 3;
	    break;
	}
	buf[0] = buf[0] | SPS15 | GAIN1;

	i2CDevice->writeI2C(buf, 1);

}

ADCDevice::~ADCDevice()
{
	delete i2CDevice;
}
