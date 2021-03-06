//
// C++ Interface: MenuScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STARTSCREENIMPL_H
#define STARTSCREENIMPL_H

#include "ui_menuscreen.h"
#include "types.h"
class QTranslator;
class QMainWindow;
class InfoScreen;
class InspectionScreen;
class SelectScreen;
class SensorScreen;
class QSettings;
class Filter;
class Buzzer;
class QTime;


class MenuScreen: public QMainWindow, public Ui::menuScreen
{
Q_OBJECT

public:
	MenuScreen(QTranslator* trans, Filter* filter, Buzzer* buzzer,
			 bool serviceKey, QWidget * parent = 0, Qt::WFlags f = 0);

public slots:
	void showMenuScreen(MenuScreenType menuScreenType);
	void levelValue(float value);

signals:
	void SendShowOperationScreen(OperationScreenType operationScreenType);
	void SendShowMenuScreen(MenuScreenType menuScreenType);
	void SendShowSelectScreen();

private:
        InfoScreen* infoScreen;
	InspectionScreen* inspectionScreen;
	SelectScreen* selectScreen;
	SensorScreen* sensorScreen;
	QListWidgetItem * menuItem;
	MenuScreenType menuScreenType;
	void doMainView();
	void doOptionView();
	void doLanguageView();
	void doLabels();
	QSettings* settings;
	QSettings* system;
	QTranslator* translator;
	bool noShowed;
	Filter* filter;
        Buzzer* mBuzzer;
	bool serviceKey;

protected:
	void keyPressEvent( QKeyEvent * event );
};

#endif
