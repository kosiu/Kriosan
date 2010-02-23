//
// C++ Implementation: InfoScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#include "infoscreen.h"
#include <QKeyEvent>
#include <QUrl>
#include <QDate>
#include <QScrollBar>
#include <QString>
#include <QSettings>
#include <QSlider>
#include <QSettings>
#include "buzzer.h"

InfoScreen::InfoScreen(Buzzer* buzzer, QWidget *parent, Qt::WFlags f)
    :QMainWindow(parent, f)
{
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
        mBuzzer = buzzer;
	
	setupUi(this);
	backLabel->setText(QString(trUtf8("Wyjście")));
	//QTextDocument infoText();

	QSettings system("/home/kosiu/system.ini", QSettings::IniFormat);

	QSettings settings("/home/kosiu/kriosan.ini", QSettings::IniFormat);
	QString language = settings.value("language", "pl").toString();

	if (language == "en") {
		textBrowser->setSource(QUrl("../resources/publicinfo_en.html"));
		titleBrowser->setSource(QUrl("../resources/kriosystem_en.html"));
	};
	if (language == "de") {
		textBrowser->setSource(QUrl("../resources/publicinfo_de.html"));
		titleBrowser->setSource(QUrl("../resources/kriosystem_de.html"));
	};
	if (language == "pl") {
		textBrowser->setSource(QUrl("../resources/publicinfo_pl.html"));
		titleBrowser->setSource(QUrl("../resources/kriosystem_pl.html"));
	};

	//Replacing teksts
	if(textBrowser->find("$DATE$")||
           textBrowser->find("$DATE$",QTextDocument::FindBackward)){
		textBrowser->cut();
		QDate date = QDate::currentDate();
		textBrowser->insertPlainText(date.toString());
	}
	if(textBrowser->find("$TIME$")||
	   textBrowser->find("$TIME$",QTextDocument::FindBackward)){
		textBrowser->cut();
		QTime time = QTime::currentTime();
		textBrowser->insertPlainText(time.toString());
	}
	if(textBrowser->find("$SOFTWARE_VERSION$")||
	   textBrowser->find("$SOFTWARE_VERSION$",QTextDocument::FindBackward)){
		textBrowser->cut();
		QString software_version = system.value("Version", "unknown").toString();
		textBrowser->insertPlainText(software_version);
	}
	if(textBrowser->find("$INSPECTION_DATE$")||
	   textBrowser->find("$INSPECTION_DATE$",QTextDocument::FindBackward)){
		textBrowser->cut();
		QDate date = system.value("inspection",QDate(2000,1,1)).toDate();
		textBrowser->insertPlainText(date.toString());
	}
	textBrowser->moveCursor(QTextCursor::Start);
}

void InfoScreen::keyPressEvent( QKeyEvent * event )
{
        mBuzzer->beep();
	//ESCAPE SWITCH
        if (event->key() == Qt::Key_Escape) {
		close();
        }
        if (event->key() == Qt::Key_Backtab) {
		textBrowser->verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
        }
        if (event->key() == Qt::Key_Tab) {
		textBrowser->verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
        }	
}

