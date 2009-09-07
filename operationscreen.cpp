//
// C++ Implementation: OperationScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2008, 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QMessageBox>
#include <QPushButton>
#include "operationscreen.h"
#include "scriptwidget.h"
#include "buzzer.h"
#include "adcdevice.h"
#include "heater.h"

OperationScreen::OperationScreen( ADCDevice*, Buzzer* buzz, QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	buzzer = buzz;
	
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);

	operationTimer = new QTimer(this);

	buttonState = start;

	heater = new Heater();

	setupUi(this);
	levelView->setScript("../resources/levelView.qjs");
	powerView->setScript("../resources/powerView.qjs");
	timeView->setScript("../resources/timeView.qjs");

	settings = new QSettings("/home/kosiu/kriosan.ini", QSettings::IniFormat);
	system = new QSettings("/home/kosiu/system.ini", QSettings::IniFormat);

	connect(operationTimer, SIGNAL(timeout()), this, SLOT(computeTime()));
}

void OperationScreen::showOperationScreen(OperationScreenType operationType)
{
	doLabels();
	powerEdit->setFocus();
	timeEdit->setFocusPolicy(Qt::WheelFocus);
	operationScreenType = operationType.operationType;
	QTime time = *(operationType.time);
	timeEdit->setTime(time);
	qDebug(qPrintable(time.toString("mm:ss")));
	powerEdit->setValue(operationType.power);

	switch(operationScreenType){
	    case 0:
		menuNameLabel->setText(trUtf8("Praca ciągła"));
		timeEdit->setFocusPolicy(Qt::NoFocus);
	    break;
	    case 1:
		menuNameLabel->setText(trUtf8("Bark"));
	    break;
	    case 2:
		menuNameLabel->setText(trUtf8("Łokieć"));
	    break;
	    case 3:
		menuNameLabel->setText(trUtf8("Nadgarstek"));
	    break;
	    case 4:
		menuNameLabel->setText(trUtf8("Biodro"));
	    break;
	    case 5:
		menuNameLabel->setText(trUtf8("Kolano"));
	    break;
	    case 6:
		menuNameLabel->setText(trUtf8("Kostka"));
	    break;
	}
	show();
}
void OperationScreen::saveSettings()
{/*
	switch (operationScreenType){
	    case 0:
		settings->setValue("operation power 0", powerEdit->value());
	    break;
	    case 1:
		settings->setValue("operation time 1", timeEdit->time());
		settings->setValue("operation power 1", powerEdit->value());
	    break;
	    case 2:
		settings->setValue("operation time 2", timeEdit->time());
		settings->setValue("operation power 2", powerEdit->value());
	    break;
	    case 3:
		settings->setValue("operation time 3", timeEdit->time());
		settings->setValue("operation power 3", powerEdit->value());
	    break;
	    case 4:
		settings->setValue("operation time 4", timeEdit->time());
		settings->setValue("operation power 4", powerEdit->value());
	    break;
	    case 5:
		settings->setValue("operation time 5", timeEdit->time());
		settings->setValue("operation power 5", powerEdit->value());
	    break;
	    case 6:
		settings->setValue("operation time 6", timeEdit->time());
		settings->setValue("operation power 6", powerEdit->value());
	    break;
	}
	//TODO hide saveButton
*/
}

void OperationScreen::doLabels()
{
	backLabel->setText(trUtf8("Wyjście"));
	selectLabel->setText(trUtf8("Start"));
	timeLabel->setText(trUtf8("Czas zabiegu:"));
	powerLabel->setText(trUtf8("Intensywność:"));
	levelLabel->setText(trUtf8("Poziom azotu:"));
}

void OperationScreen::levelValue(int value)
{
	if(isActiveWindow()){
		value = (value /16);
		int min =  system->value("Bottle_Min", 0.00).toDouble() * 1000.0;
		int max =  system->value("Bottle_Max", 2.04).toDouble() * 1000.0;
		value = (100 * (value - min)) / (max - min);
		if (value>100) value = 100; if (value<0) value = 0;

		static float lastValue1 = value;
		static float lastValue2 = value;
		static float mediumValue = value;

		float newValue = (lastValue1 + lastValue2 + value)/3;

		levelEdit->setValue(newValue);
		//qDebug("%d",value);
		levelView->setPName("value");
		levelView->setPValue(newValue);

		mediumValue = newValue;
		lastValue2 = lastValue1;
		lastValue1 = value;
	}
}

void OperationScreen::levelHeat()
{
	int heat;
	if ( buttonState == start) heat=0; else heat=1;
	levelView->setPName("heat");
	levelView->setPValue(heat);
}

void OperationScreen::powerValue(int value)
{
	powerView->setPName("val");
	powerView->setPValue(value);
	heater->updatePower(powerEdit->value());
	//TODO emit show saveButton
}

void OperationScreen::timeValue(QTime value)
{
	int val = value.second() + 60 * value.minute();
	timeView->setPName("val");
	timeView->setPValue(val/2);
	if (buttonState == stop) {
		//TODO emit show saveButton
	}
}

void OperationScreen::startOperation()
{	qDebug("czujnik: %f, prog: %f",temp,system->value("Temperature",77).toDouble());
	if (temp<system->value("Temperature",77).toDouble()){
		heater->turnOn(powerEdit->value());
		buzzer->beep(70);
		setStyleSheet(
			"QMainWindow{background-image: url(../resources/background2.png);}");
		buttonState = stop;
		selectLabel->setText(tr("Stop"));
		levelView->setPName("heat");
		levelView->setPValue(1);
		lastTime = timeEdit->time();
		//Disable focus
		timeEdit->setFocusPolicy(Qt::NoFocus);
		timeEdit->clearFocus();
		if(operationScreenType != 0){
			timeLabel->setText(trUtf8("Pozostały czas:"));
		};
			operationTimer->start(1000);
	}
	else
	{
		buttonState = stop;
		overheat(0);
	}
}

void OperationScreen::stopOperation()
{	
	if(buttonState==stop){
		heater->turnOff();
		buzzer->beep(100);
		setStyleSheet(
			"QMainWindow{background-image: url(../resources/background.png);}");
		operationTimer->stop();
		buttonState = start;
		selectLabel->setText(tr("Start"));
		levelView->setPName("heat");
		levelView->setPValue(0);
		timeEdit->setTime(lastTime);
		timeLabel->setText(trUtf8("Czas zabiegu:"));
		//Enablig focus
		if(operationScreenType != 0){
			timeEdit->setFocusPolicy(Qt::WheelFocus);
		}
	}
}

void OperationScreen::computeTime()
{
	QTime acctualTime;
	QString str;
	//buzzer->beep(5);<---------------------------------------buzzer
	acctualTime = timeEdit->time();
	if (operationScreenType == 0){
		acctualTime = acctualTime.addSecs(1);
	}else{
		acctualTime = acctualTime.addSecs(-1);
	}
	timeEdit->setTime(acctualTime);
	//buzzer->beep(10);
	if (acctualTime <= QTime(0,0))
		emit(stopOperation());
}

void OperationScreen::overheat(int voltage)
{
	if(isActiveWindow()){
		//celc = 2.3412 * R - 243.4192
		// R = 1000 * U / (14.19 - U)
		temp = 0.01067 * voltage + 34.0886;
		if(buttonState==stop){
			if (temp>=system->value("Temperature",77).toDouble()){
				emit(stopOperation());
				QMessageBox* messageBox = new QMessageBox(QMessageBox::Critical, "info", trUtf8("Za mało azotu"), QMessageBox::Cancel );
				messageBox->setDefaultButton(QMessageBox::Cancel);
				messageBox->defaultButton()->setText(trUtf8("Wyjście"));
				messageBox->setWindowFlags(Qt::FramelessWindowHint);
				messageBox->exec();
				delete messageBox;
			}
		}
	}
}

void OperationScreen::keyPressEvent( QKeyEvent * event )
{
        if (event->key() == Qt::Key_Escape) {
	    if(operationScreenType==0)
		emit( SendShowMenuScreen( mainView ));
	    else
	        emit( SendShowSelectScreen());
            hide(); stopOperation();
        }
	if (event->key() == Qt::Key_Play) {
	    //Button select pressed
	    switch (buttonState) {
		case start:
		    startOperation();
			qDebug("startOperation();");
		break;
		case stop:
		    stopOperation();
			qDebug("stopOperation();");
		break;
	    }
	}
}



//void OperationScreen::powerValue(int value)
//{
//	qDebug("wartosc do wysłania %d", value);
//	levelView->setPName("value");
//	qDebug("PName: %s", qPrintable(levelView->getPName()));
//	qDebug("PValue: %s", qPrintable(levelView->getPValue().toString()));
//
//	levelView->setPValue(value);
//
//	qDebug("PName: %s", qPrintable(levelView->getPName()));
//	qDebug("PValue: %s", qPrintable(levelView->getPValue().toString()));
//}
