//
// C++ Implementation: MenuScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#include <QFile>
#include <QListWidget>
#include <QKeyEvent>
#include <QTranslator>
#include <QString>
#include <QSettings>
#include <QMessageBox>
#include <QPushButton>
#include <QProcess>
#include <QTime>
#include <QDate>

#include "menuscreen.h"
#include "infoscreen.h"
#include "inspectionscreen.h"
#include "selectscreen.h"
#include "sensorscreen.h"
#include "buzzer.h"
#include "adcdevice.h"

MenuScreen::MenuScreen(QTranslator* trans, ADCDevice* adc_readArg, Buzzer* buzzerArg,
			 bool serviceKeyArg, QWidget * parent, Qt::WFlags f)
    :QMainWindow(parent, f)
{
	buzzer=buzzerArg;
	adc_read=adc_readArg;
	serviceKey = serviceKeyArg;
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
	
	translator = trans;

	setupUi(this);
	levelView2->setScript("../resources/levelView2.qjs" );

	system = new QSettings("/home/kosiu/system.ini", QSettings::IniFormat);
	settings = new QSettings("/home/kosiu/kriosan.ini", QSettings::IniFormat);

	menuScreenType = mainView;
	noShowed = true;
	doMainView();
}

void MenuScreen::levelValue(int value)
{
	if(isActiveWindow()){
		value = (value /16);
		int min =  system->value("Bottle_Min", 0.00).toDouble() * 1000.0;
		int max =  system->value("Bottle_Max", 2.04).toDouble() * 1000.0;
		value = (100 * (value - min)) / (max - min);
		if (value>100) value = 100; if (value<0) value = 0;

		levelEdit->setValue(value);
		levelView2->setPName("value");
		levelView2->setPValue(value);
	}
}

void MenuScreen::showMenuScreen(MenuScreenType menuType)
{
	menuScreenType = menuType;
	switch(menuScreenType){
		case mainView:      doMainView();      break;
		case optionView:    doOptionView();    break;
		case languageView:  doLanguageView();  break;
	}
	show();
}

void MenuScreen::doLabels()
{
	if (system->value("Exit", 1).toInt() == 0 && menuScreenType == mainView){
		backLabel->hide();
	} else {
		backLabel->show();
	}
	selectLabel->setText(trUtf8("Wybierz"));
	levelLabel->setText(trUtf8("Poziom azotu:"));	
	backLabel->setText(trUtf8("Wyjście"));
}

void MenuScreen::doMainView()
{
	menuView->clear();
	doLabels();
	if(noShowed){
		show();

		//computing inspetion date
		QDate actualDate;
		actualDate = QDate::currentDate();
		QDate inspectionDate = system->value("inspection",QDate(2000,1,1)).toDate();
		int daysLeft = actualDate.daysTo(inspectionDate);

		if (daysLeft < 10){
			QMessageBox::Icon icon;
			QString infText;
			if(daysLeft < 0){
				icon = QMessageBox::Critical;
				infText=QString(trUtf8("Upłynęło %1 dni od planowanego przeglądu").arg(daysLeft*-1));
			} else {
				icon = QMessageBox::Information;
				infText=QString(trUtf8("Do przeglądu pozostało %1 dni").arg(daysLeft));
			}
			QMessageBox* messageBox = new QMessageBox(icon, "info",infText, QMessageBox::Cancel );
			backLabel->show();
			messageBox->setDefaultButton(QMessageBox::Cancel);
			messageBox->defaultButton()->setText(trUtf8("Wyjście"));
			messageBox->setWindowFlags(Qt::FramelessWindowHint);
			//messageBox->show();
			messageBox->exec();
			delete messageBox;
			backLabel->hide();

		}
		noShowed = false;
	}
	menuNameLabel->setText(trUtf8("Menu Startowe"));

	menuItem = new QListWidgetItem(trUtf8("Praca ciągła"), menuView);
	menuItem->setIcon(QIcon("../resources/continues.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);
	
	menuItem = new QListWidgetItem(trUtf8("Zabieg zdefiniowany"), menuView);
	menuItem->setIcon(QIcon("../resources/history.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);

	menuItem = new QListWidgetItem(trUtf8("Opcje"), menuView);
	menuItem->setIcon(QIcon("../resources/configure.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);

	menuView->setCurrentRow(0);
}

void MenuScreen::doOptionView(){
	menuView->clear();
	doLabels();
	menuNameLabel->setText(trUtf8("Opcje"));

	menuItem = new QListWidgetItem(trUtf8("Wybór języka"), menuView);
	menuItem->setIcon(QIcon("../resources/translator.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);
	
	menuItem = new QListWidgetItem(trUtf8("Przywracanie ustawień"), menuView);
	menuItem->setIcon(QIcon("../resources/undo.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);

	menuItem = new QListWidgetItem(trUtf8("Informacje"), menuView);
	menuItem->setIcon(QIcon("../resources/help.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);

	if(serviceKey)
	{
		menuItem = new QListWidgetItem(trUtf8("Termin przeglądu"), menuView);
		menuItem->setIcon(QIcon("../resources/configure.png"));
		menuItem->setTextAlignment(Qt::AlignHCenter);
	
		menuItem = new QListWidgetItem(trUtf8("Kalibracja"), menuView);
		menuItem->setIcon(QIcon("../resources/configure.png"));
		menuItem->setTextAlignment(Qt::AlignHCenter);
	}
	menuView->setCurrentRow(0);
}

void MenuScreen::doLanguageView(){
	menuView->clear();
	doLabels();
	menuNameLabel->setText(trUtf8("Język"));

	menuItem = new QListWidgetItem(trUtf8("Polski"), menuView);
	menuItem->setIcon(QIcon("../resources/pl.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);
	
	menuItem = new QListWidgetItem(trUtf8("Angielski"), menuView);
	menuItem->setIcon(QIcon("../resources/gb.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);

	menuItem = new QListWidgetItem(trUtf8("Niemiecki"), menuView);
	menuItem->setIcon(QIcon("../resources/de.png"));
	menuItem->setTextAlignment(Qt::AlignHCenter);

	menuView->setCurrentRow(0);
}


void MenuScreen::keyPressEvent( QKeyEvent * event )
{
	OperationScreenType operationType;

	//ESCAPE SWITCH
        if (event->key() == Qt::Key_Escape) {qDebug("menuType ESC: %d",menuScreenType);
		switch(menuScreenType){
			case mainView:
				if (system->value("Exit", 1).toInt() == 1){
					close();
				}
				break;
			case optionView:
				emit(SendShowMenuScreen(mainView)); break;
			case languageView:
				emit(SendShowMenuScreen(optionView)); break;
		}
        }

	//SELECT SWITCH
	if (event->key() == Qt::Key_Play) {
		int menuIndex = menuView->currentRow();
		switch(menuScreenType){
		    case mainView:
			switch(menuIndex){
			    case 0:
				operationType.operationType = 0;
				operationType.power = 100;
				operationType.time = &QTime(0,0,0);
				emit(SendShowOperationScreen(operationType));
				lower(); clearFocus ();
			    break;
			    case 1:
				emit(SendShowSelectScreen());
			    break;
			    case 2:
				emit(SendShowMenuScreen(optionView));
			    break;
			};
		    break;
		    case optionView:
			switch(menuIndex){
			    case 0:
				emit(SendShowMenuScreen(languageView));
			    break;
			    case 1:
				settings->clear();
				qApp->removeTranslator(translator);
				emit(SendShowMenuScreen(mainView));
			    break;
			    case 2:
				//Public Information
				infoScreen = new InfoScreen();
				infoScreen->show();
			    break;
			    case 3:
				//Inspection
				inspectionScreen = new InspectionScreen();
				inspectionScreen->show();
			    break;
			    case 4:
				//Calibration
				sensorScreen = new SensorScreen(adc_read, buzzer);
				sensorScreen->show();
			    break;
			};
		    break;
		    case languageView:
			switch(menuIndex){
			    case 0:
				//Set Polisch language
				settings->setValue("language", QString("pl"));
				qApp->removeTranslator(translator);
				emit(SendShowMenuScreen(optionView));
			    break;
			    case 1:
				//Set Englisch language
				settings->setValue("language", QString("en"));
				qApp->removeTranslator(translator);
				translator->load("kriosan_en");
				qApp->installTranslator(translator);
				emit(SendShowMenuScreen(optionView));
			    break;
			    case 2:
				//Set German language
				settings->setValue("language", QString("de"));
				qApp->removeTranslator(translator);
				translator->load("kriosan_de");
				qApp->installTranslator(translator);
				emit(SendShowMenuScreen(optionView));
			    break;
			    settings->sync();
			    QProcess sync;
			    sync.start("/bin/sync");
			    sync.waitForFinished();

			};
		    break;
		}

	}
}

