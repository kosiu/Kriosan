//
// C++ Interface: SelectScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#ifndef SELECTSCREEN_H
#define SELECTSCREEN_H

#include "ui_selectscreen.h"
#include "types.h"
class QMainWindow;
class QSettings;
class QTime;
class Buzzer;

class SelectScreen: public QMainWindow, public Ui::selectScreen
{
Q_OBJECT

public:
        SelectScreen(Buzzer* buzzer, QWidget * parent = 0, Qt::WFlags f = 0);

public slots:
	void showSelectScreen();

signals:
	void SendShowOperationScreen(OperationScreenType);
	void SendShowMenuScreen(MenuScreenType menuScreenType);

private:
	int part;
	int size;
	void updateStatus();
	QSettings* settings;
	int power;
	QTime time;
        Buzzer* mBuzzer;

protected:
	void keyPressEvent( QKeyEvent * event );
	bool event(QEvent * event);
};

#endif
