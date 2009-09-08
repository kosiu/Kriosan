//
// C++ Implementation: Filter
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "filter.h"
#include "adcdevice.h"
#include <QSettings>


Filter::Filter(QObject *parent) : QObject(parent)
{
	adc_read.singleConversion(1);
	connect(&adc_read,SIGNAL(channel1(int)),this,SLOT(channel1(int)));
	connect(&adc_read,SIGNAL(channel2(int)),this,SLOT(channel2(int)));
	connect(&adc_read,SIGNAL(channel3(int)),this,SLOT(channel3(int)));
}


Filter::~Filter()
{
}


void Filter::channel1(int value)
{
	float voltage;
	voltage = (float)value / 16000.0; 
	emit keyValue(voltage);
}

void Filter::channel2(int value)
{
	//celc = 2.3412 * R - 243.4192
	// R = 1000 * U / (14.19 - U)
	float temp;
	temp = 0.01067 * (float)value + 34.0886;
	emit tempValue(temp);
}

void Filter::channel3(int value)
{
	QSettings *system = new QSettings("/home/kosiu/system.ini", QSettings::IniFormat);

	int min =  system->value("Bottle_Min", 0.00).toDouble() * 16000;
	int max =  system->value("Bottle_Max", 2.04).toDouble() * 16000;
	value = (100 * (value - min)) / (max - min);
	if (value>100) value = 100; if (value<0) value = 0;

	float level = value;
	emit levelValue(level);
}

ADCDevice Filter::adc_read;

