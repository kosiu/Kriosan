//
// C++ Implementation: Buzzer
//
// Description:
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QString>
#include <QSettings>
#include <QFile>
#include "buzzer.h"
#include "types.h"

Buzzer::Buzzer(QObject *parent)
   : QThread(parent), quit(false)
{
}

Buzzer::~Buzzer()
{
	quit = true;
	cond.wakeOne();
	wait();
}

void Buzzer::beep(int msecArg) {
	QMutexLocker locker(&mutex);
		msec = msecArg;
	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void Buzzer::run()
{
	mutex.lock();
		int msecTime = msec;
	mutex.unlock();

        QSettings system(SysConfigFileName, QSettings::IniFormat);

	QString gpioName = QString("/proc/gpio/GPIO") +
		QString(system.value("beeperGPIO","16").toString());
	qDebug("nazwa: %s",qPrintable(gpioName));

	QFile file(gpioName);
	file.open( QIODevice::WriteOnly|QIODevice::Text| QIODevice::Unbuffered );

	file.write("GPIO",5);
	file.write("out",4);

	while (!quit) {

		file.write("set",4);
		msleep(msecTime);
		file.write("clear",6);
	
		QMutexLocker locker(&mutex);
         	cond.wait(&mutex);

         	msecTime = msec;
	}
}
