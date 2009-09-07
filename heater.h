//
// C++ Interface: Heater
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef HEATER_H
#define HEATER_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
class QFile;

class Heater : public QThread
{
Q_OBJECT

public:
	Heater(QObject *parent = 0, const char *name = 0);
	~Heater();	

public slots:
	void turnOn(int power);
	void turnOff();
	void updatePower(int power);

private:
	QWaitCondition cond;
	QMutex mutex;
	bool quit;
	bool heaterOn;
	int power;

protected:
     void run();
};

#endif
