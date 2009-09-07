//
// C++ Interface: OperationScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2008, 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include "ui_operationscreen.h"
#include "types.h"

class QTime;
class QEvent;
class QSettings;
class QMainWindow;
class ADCDevice;
class Buzzer;
class Heater;

class OperationScreen : public QMainWindow, public Ui::operationScreen
{
Q_OBJECT

public:
	OperationScreen( ADCDevice*, Buzzer*, QWidget * parent = 0, Qt::WFlags f = 0 );

public slots:
	void levelValue(int value);
	void showOperationScreen(OperationScreenType);
	void overheat(int adc_read);

signals:
	void SendShowMenuScreen(MenuScreenType menuScreenType);
	void SendShowSelectScreen();

private slots:
	void levelHeat();
	void powerValue(int value);
	void timeValue(QTime value);
	void startOperation();
	void stopOperation();
	void computeTime();

private:
	enum {start, stop} buttonState;
	QTime lastTime;
	QTimer* operationTimer;
	int operationScreenType;
	void doLabels();
	QSettings* settings;
	QSettings* system;
	Buzzer* buzzer;
	Heater* heater;
	float temp;

protected:
	void keyPressEvent( QKeyEvent * event );
};
#endif
