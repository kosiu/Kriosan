//
// C++ Interface: SensorScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#ifndef SENSORSCREEN_H
#define SENSORSCREEN_H

#include "ui_sensorsscreen.h"
class QMainWindow;
class Filter;
class Buzzer;
class QSettings;

class SensorScreen: public QMainWindow, public Ui::sensorScreen
{
Q_OBJECT

public:
	SensorScreen(Filter* filter, Buzzer* buzzer,QWidget * parent = 0, Qt::WFlags f = 0);

protected:
	void keyPressEvent( QKeyEvent * event );

private slots:
	void levelVoltage(int);
	void temperatureVoltage(float);
	void keyVoltage(float);

private:
	Filter* filter;
	QSettings* system;
        Buzzer* mBuzzer;
};

#endif
