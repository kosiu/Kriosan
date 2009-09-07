//
// C++ Implementation: Heater
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "heater.h"
#include <QSettings>
#include <QFile>

Heater::Heater(QObject *parent, const char *name)
	:QThread(parent), quit(false)
{
}

Heater::~Heater()
{
	quit = true;
	heaterOn = false;
	cond.wakeOne();
	wait();
}

void Heater::updatePower(int power)
{
	QMutexLocker locker(&mutex);
	Heater::power = power;
	cond.wakeOne();
}

void Heater::turnOn(int power)
{
	QMutexLocker locker(&mutex);
	Heater::power = power;
	heaterOn = true;

	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void Heater::turnOff()
{
	heaterOn = false;
	cond.wakeOne();
}

void Heater::run()
{
	mutex.lock();
		int pwmWidth = power;
		bool heating = heaterOn;
	mutex.unlock();

	QSettings system("/home/kosiu/system.ini", QSettings::IniFormat);

	QString gpioName = QString("/proc/gpio/GPIO") + QString(system.value("heaterGPIO","17").toString());
	qDebug("nazwa: %s",qPrintable(gpioName));

	QFile file(gpioName);
	file.open( QIODevice::WriteOnly|QIODevice::Text| QIODevice::Unbuffered );

	file.write("GPIO",5);
	file.write("out",4);

	while (!quit) {

		while(heaterOn) {
			if (pwmWidth>0){
				file.write("set",4);
				msleep(pwmWidth*3);
			}
			if (pwmWidth<100){
				file.write("clear",6);
				msleep((100-pwmWidth)*3);
			}
			QMutexLocker locker(&mutex);
			pwmWidth = power;
		}
		file.write("clear",6);
		QMutexLocker locker(&mutex);
         	cond.wait(&mutex);
		pwmWidth = power;
	}

}
