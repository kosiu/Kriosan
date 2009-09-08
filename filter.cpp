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
#include <QSettings>

Filter::Filter(QObject *parent)
	: QThread(parent), quit(false)
{

}


Filter::~Filter()
{
	quit = true;
	cond.wakeOne();
	wait();
}

void Filter::channel1(int value)
{
	QMutexLocker locker(&mutex);
		val = value;
		channel = 0;
	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void Filter::channel2(int value)
{
	QMutexLocker locker(&mutex);
		val = value;
		channel = 1;
	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void Filter::channel3(int value)
{
	QMutexLocker locker(&mutex);
		val = value;
		channel = 2;
	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void Filter::run()
{
	mutex.lock();
		int l_value = val;
		int l_channel = channel;
	mutex.unlock();

	QSettings *system = new QSettings("/home/kosiu/system.ini", QSettings::IniFormat);

	while (!quit) {
		//sending signal:
		switch(l_channel)
		{
			case 0:
				float voltage;
				voltage = (float)l_value / 16000.0; 
				emit keyValue(voltage);
			break;
			case 1:
				//celc = 2.3412 * R - 243.4192
				// R = 1000 * U / (14.19 - U)
				float temperature;
				temperature = 0.01067 * (float)l_value + 34.0886;
				emit temperatureValue(temperature);
			break;
			case 2:
				int min =  system->value("Bottle_Min", 0.00).toDouble() * 16000;
				int max =  system->value("Bottle_Max", 2.04).toDouble() * 16000;
				l_value = (100 * (l_value - min)) / (max - min);
				if (l_value>100) l_value = 100; if (l_value<0) l_value = 0;

				float level = l_value;
				emit levelValue(level);
			break;
		};	
		//wait for signal
		QMutexLocker locker(&mutex);
         	cond.wait(&mutex);

		l_value = val;
		l_channel = channel;
	}
}
