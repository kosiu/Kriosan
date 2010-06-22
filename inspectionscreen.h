//
// C++ Interface: InspectionScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INSPECTIONSCREEN_H
#define INSPECTIONSCREEN_H

#include "ui_inspectionscreen.h"
class QMainWindow;
class QSettings;
class Buzzer;
class QDateTime;

class InspectionScreen: public QMainWindow, public Ui::inspectionScreen
{
Q_OBJECT

public:
        InspectionScreen(Buzzer* buzzer, QWidget * parent = 0, Qt::WFlags f = 0);

private:
	QSettings* system;
        Buzzer* mBuzzer;
	void setTime( QDateTime dateTime);

private slots:
	void changeSelectName(QWidget * old, QWidget * now);

protected:
	void keyPressEvent( QKeyEvent * event );
};

#endif
