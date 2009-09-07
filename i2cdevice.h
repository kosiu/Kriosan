//
// C++ Interface: I2CDevice
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef I2CDEVICE_H
#define I2CDEVICE_H

class I2CDevice{

public:
	I2CDevice(int addressOfDevice);
	enum {ok, busError, deviceError, writeError, readError} state;

	void writeI2C(char* bufor, int length);
	void readI2C(char* bufor, int length);

	~I2CDevice();

private:
	int address;
	int file;
};

#endif
