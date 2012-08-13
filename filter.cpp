//
// C++ Implementation: Filter
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009, 2012
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "filter.h"
#include "adcdevice.h"
#include <QSettings>
#include "QtDebug"
#include "types.h"

Filter::Filter(QObject *parent) : QObject(parent)
{
	mVoltage = 0;
	mLevel = 0;
	mTemperature = 0;
	brokenTemperatureSensor = false;

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
	mVoltage = voltage;
	emit keyValue(voltage);
}

void Filter::channel2(int value)
{

	static int bufor[10]={0,0,0,0,0,0,0,0,0,0};
	static int index = 0;
	static bool first_iteration = true;
	
	value = computeFilter(value, bufor, index, first_iteration, 10);

	//celc = 2.3412 * R - 243.4192
	// R = 1000 * U / (14.19 - U)
	float temp;
	temp = 0.01067 * (float)value + 34.0886;
	mTemperature = temp;
	if (brokenTemperatureSensor == true) temp = -1;
	emit tempValue(temp);
}

void Filter::channel3(int value)
{
	static int bufor[10]={0,0,0,0,0,0,0,0,0,0};
	static int index = 0;
	static bool first_iteration = true;
	
	value = computeFilter(value, bufor, index, first_iteration, 10);

        QSettings *system = new QSettings(SysConfigFileName, QSettings::IniFormat);

	int min =  system->value("Bottle_Min", 0.00).toDouble() * 16000;
	int max =  system->value("Bottle_Max", 2.04).toDouble() * 16000;
	value = (100 * (value - min)) / (max - min);
	if (value>100) value = 100; if (value<0) value = 0;

	float level = value;

	mLevel = level;
	emit levelValue(level);
}

float Filter::computeFilter(int &value, int bufor[],
				int &index, bool& first_iteration, int lenght)
{
	bufor[index] = value;
	value = 0;

	if ( (index + 1) == lenght )
	{
		first_iteration = false;
		index = -1;
	}

	if (first_iteration) lenght = index + 1;

	for (int i=0;i<lenght;i++)
	{
		value = value + bufor[i];
	}
	value = value / lenght;

	index++;
	return value;
}

ADCDevice Filter::adc_read;

