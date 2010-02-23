//
// C++ Implementation: Main
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2007, 2008, 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QtGui>
#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QString>

#include "operationscreen.h"
#include "menuscreen.h"
#include "types.h"
#include "adcdevice.h"
#include "buzzer.h"
#include "selectscreen.h"
#include "filter.h"

#ifdef __arm__
#include <QKbdDriverFactory>
Q_IMPORT_PLUGIN(ustandardkbddriver);
#endif



int main(int argc, char ** argv)
{
	QApplication app( argc, argv );

	//Reading language settings and translator installing
        QSettings settings(UserConfigFileName, QSettings::IniFormat);
        QSettings system(SysConfigFileName, QSettings::IniFormat);
	QTranslator translator;
	QString language = settings.value("language", "pl").toString();
	if (language == "en") {translator.load("kriosan_en");qDebug("angielski");}
	if (language == "de") translator.load("kriosan_de");
	if (language != "pl") app.installTranslator(&translator);


	//Keyboard load
#ifdef __arm__
	QStringList ListOfKeyboardsDrivers;
	QKbdDriverFactory* KeyboardDriverFactory;
        KeyboardDriverFactory->create("Kriosan","o");

	ListOfKeyboardsDrivers=KeyboardDriverFactory->keys();
	QString  keyboardItem;
	for (int i=0;i<ListOfKeyboardsDrivers.count();i++)
	{
		keyboardItem=ListOfKeyboardsDrivers[i];
		qDebug("Keyboard driver %d of %d: %s", i+1,
			ListOfKeyboardsDrivers.count(), qPrintable(keyboardItem));

	}
#endif

	//Style sheat instalation
	QFile styleSheet("../resources/style.qss");
    	if (!styleSheet.open(QIODevice::ReadOnly)) {
        	qWarning("Unable to open ../resources/style.qss");
    	}
    	qApp->setStyleSheet(styleSheet.readAll());

	//checking if inspection key is pluged
	Filter* filter;
	filter = new Filter();
	bool serviceKey=false;
	int value = filter->adc_read.singleConversion(1);
	float min = system.value("Key_Min", 0.4).toDouble();
	float max = system.value("Key_Max", 2.04).toDouble();
	float voltage = (value /16) / 1000.0;
	if(voltage>min && voltage<max) serviceKey = true;
	qDebug()<<"service"<<serviceKey<<voltage;


	//aplication start
	Buzzer* buzzer;
	buzzer = new Buzzer();

	filter->adc_read.startConversions(2 , 250);

	//Windows initialisation
	OperationScreen operationScreen(filter, buzzer);
        SelectScreen* selectScreen = new SelectScreen(buzzer);
	MenuScreen* menuScreen = new MenuScreen(&translator, filter, buzzer, serviceKey);
	//some IFDEF should be
	menuScreen->setGeometry(0,0,240,320);
	operationScreen.setGeometry(0,0,240,320);
	//setWindowFlags(menuScreendowFlags()|Qt::FramelessWindowHint);


	//making signals connections
	app.connect(filter, SIGNAL(levelValue(float)), &operationScreen, SLOT(levelValue(float)));
	app.connect(filter, SIGNAL(levelValue(float)), menuScreen, SLOT(levelValue(float)));
	app.connect(filter, SIGNAL(tempValue(float)), &operationScreen, SLOT(overheat(float)));


	//menu -> operation (continues operation)
	app.connect(menuScreen,      SIGNAL(SendShowOperationScreen(OperationScreenType) ), 
		    &operationScreen,  SLOT(showOperationScreen(OperationScreenType)));

	//select -> operation (normal operation)
	app.connect(selectScreen,      SIGNAL(SendShowOperationScreen(OperationScreenType) ), 
		    &operationScreen,    SLOT(showOperationScreen(OperationScreenType)));

	//operation -> menu (back from continues operation)
	app.connect(&operationScreen,SIGNAL(SendShowMenuScreen(MenuScreenType) ),
                    menuScreen,        SLOT(showMenuScreen(MenuScreenType)));

	//operation -> select (back from normal operation)
	app.connect(&operationScreen,      SIGNAL(SendShowSelectScreen() ), 
		    selectScreen,           SLOT(showSelectScreen()));

	//select -> menu
	app.connect(selectScreen,SIGNAL(SendShowMenuScreen(MenuScreenType) ),
                    menuScreen,        SLOT(showMenuScreen(MenuScreenType)));

	//menu -> select
	app.connect(menuScreen,      SIGNAL(SendShowSelectScreen() ), 
		    selectScreen,    SLOT(showSelectScreen()));

	//menu -> menu
	app.connect(menuScreen,     SIGNAL(SendShowMenuScreen(MenuScreenType) ),
                    menuScreen,       SLOT(showMenuScreen(MenuScreenType)));

	//menuScreen->show();

	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
