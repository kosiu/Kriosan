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
#include <QTime>
#include <QKeyEvent>
#include <QEvent>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QMessageBox>
#include <QPushButton>
#include "operationscreen.h"
#include "scriptwidget.h"
#include "buzzer.h"
#include "filter.h"
#include "heater.h"

OperationScreen::OperationScreen( Filter*, Buzzer* buzz, QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
        mBuzzer = buzz;
	
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);

	operationTimer = new QTimer(this);

	buttonState = start;

	heater = new Heater();

	setupUi(this);
	levelView->setScript("../resources/levelView.qjs");
	powerView->setScript("../resources/powerView.qjs");
	timeView->setScript("../resources/timeView.qjs");

        settings = new QSettings(UserConfigFileName, QSettings::IniFormat);
        system = new QSettings(SysConfigFileName, QSettings::IniFormat);
	
	timeEdit->setMaximumTime(QTime(0,59,0));
	timeEdit->setMinimumTime(QTime(0,0,0));

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

void OperationScreen::doLabels()
{
	backLabel->setText(trUtf8("Wyjście"));
	selectLabel->setText(trUtf8("Start"));
	timeLabel->setText(trUtf8("Czas zabiegu"));
	powerLabel->setText(trUtf8("Intensywność"));
	levelLabel->setText(trUtf8("Poziom azotu"));
}

void OperationScreen::levelValue(float value)
{
	if(isActiveWindow()){
		levelEdit->setValue(value);
		levelView->setPName("value");
		levelView->setPValue(value);
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
}

void OperationScreen::timeValue(QTime value)
{
	int val = value.second() + 60 * value.minute();
	timeView->setPName("val");
	timeView->setPValue(val/2);
	if (buttonState == stop) {
	}
}

void OperationScreen::startOperation()
{	qDebug("czujnik: %f, prog: %f",temp,system->value("Temperature",77).toDouble());
	if (temp<system->value("Temperature",77).toDouble()){
		heater->turnOn(powerEdit->value());
                mBuzzer->beep(70);
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
			timeLabel->setText(trUtf8("Pozostały czas"));
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
                mBuzzer->beep(100);
		setStyleSheet(
			"QMainWindow{background-image: url(../resources/background.png);}");
		operationTimer->stop();
		buttonState = start;
		selectLabel->setText(tr("Start"));
		levelView->setPName("heat");
		levelView->setPValue(0);
		timeEdit->setTime(lastTime);
		timeLabel->setText(trUtf8("Czas zabiegu"));
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
	acctualTime = timeEdit->time();
	if (operationScreenType == 0){
		acctualTime = acctualTime.addSecs(1);
	}else{
		acctualTime = acctualTime.addSecs(-1);
	}
	timeEdit->setTime(acctualTime);
	if (acctualTime <= QTime(0,0))
		emit(stopOperation());
}

void OperationScreen::overheat(float tempArg)
{
	temp = tempArg;
	if(isActiveWindow()){
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
        //mBuzzer->beep();
        if (event->key() == Qt::Key_Escape) {
	    if(operationScreenType==0)
		emit( SendShowMenuScreen( mainView ));
	    else
	        emit( SendShowSelectScreen());
            hide(); stopOperation();
        }
        if ((event->key() == Qt::Key_Play)||(event->key() == Qt::Key_F2)) {
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

bool OperationScreen::event(QEvent *event){
	if (event->type() == QEvent::ShortcutOverride) {
                //mBuzzer->beep();
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		if (ke->key() == Qt::Key_Up){
		    if(timeEdit->hasFocus())
		    {
		    	qDebug("UP");
			timeEdit->setTime(timeEdit->time().addSecs(30));
		    }
		    return true;
		}
		if (ke->key() == Qt::Key_Down) {
		    if(timeEdit->hasFocus())
		    {
		    	qDebug("DOWN");
			if(timeEdit->time()==QTime(0,0,30))
				timeEdit->setTime(QTime(0,1,0));
			timeEdit->setTime(timeEdit->time().addSecs(-30));
		    }
		    return true;
		}
	}
	return QWidget::event(event);
}
