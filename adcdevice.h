//
// C++ Interface: ADCDevice
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QObject>
#ifndef ADCDEVICE_H
#define ADCDEVICE_H
class I2CDevice;
class QTimer;

class ADCDevice : public QObject
{
    Q_OBJECT

    public:
	ADCDevice();
	int singleConversion(int channel);
	void startConversions(int channels, int period_mSec);
	void stopConversion();

	~ADCDevice();

    signals:
	void channel1(int value);
	void channel2(int value);
	void channel3(int value);
	
    private slots:
	void intervalElapsed();
	
    private:
	I2CDevice* i2CDevice;
        QTimer* interval;
	int numberOfCannels;
	int currentChannel;

	//adres of device depends of pin configuration
	//  addres      pin: A0        pin: A1
	//   72           0               0
	//   73           0              n.c.
	//   74           0               1
	//   75          n.c.             0
	//   76           1               0
	//   77           1              n.c.
	//   78           1               1
	//   79          n.c.             1
	// inwalid       n.c.            n.c.

	//first byte writen to ADS1112 is configuration register
	//rest ar ignored.
	//first byte read from ADS1112 is high byte of conversion
	//secon byte read from ADS1112 is low  byte of conversion
	//tird  byte read from ADS1112 is configuration register


	//CONFIGURATION REGISTER
	//flag and mask : ST/_DRDY_
	//mask for read: old data in buffer (if 1)
	//flag for write: in single conversion mode starts conversion
	static const char DRDY = 1 << 7;

	//flags: INP0 :: INP1 (input)
	static const char AIN01 = 0 << 5;
	static const char AIN23 = 1 << 5;
	static const char AIN03 = 2 << 5;
	static const char AIN13 = 3 << 5;

	//flag: SC (enable single conversion)
	static const char SC = 1 << 4;

	//flags: DR0 :: DR1 (speed of ADC)
	static const char SPS240 = 0 << 2; //12bit range:  2 048 to  -2 047
	static const char SPS60  = 1 << 2; //14bit range:  8 192 to  -8 191
	static const char SPS30  = 2 << 2; //15bit range: 16 384 to -16 383
	static const char SPS15  = 3 << 2; //16bit range: 32 768 to -32 767

	//flags: PGA0 :: PGA1
	static const char GAIN1 = 0;
	static const char GAIN2 = 1;
	static const char GAIN4 = 2;
	static const char GAIN8 = 3;

	//computing result:
        //result = GAIN * RANGE * (V+ - V-) / 2 048V
};

#endif
