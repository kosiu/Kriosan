//
// C++ Implementation: InspectionScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#include <QKeyEvent>
#include <QSettings>
#include <QProcess>
#include <QApplication>
#include "realtimeclock.h"
#include "inspectionscreen.h"
#include "types.h"

InspectionScreen::InspectionScreen(QWidget *parent, Qt::WFlags f)
    :QMainWindow(parent, f)
{
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);

	setupUi(this);

        system = new QSettings(SysConfigFileName, QSettings::IniFormat);
	inspectionDateEdit->setDate(system->value("inspection",QDate(2000,1,1)).toDate());
	
	QDateTime date = QDateTime::currentDateTime();
	dateTimeEdit->setDateTime(date);

	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(changeSelectName(QWidget*, QWidget*)));
}

void InspectionScreen::changeSelectName(QWidget * old, QWidget * now)
{
	if (now==inspectionDateEdit) selectLabel->setText(trUtf8("Zapisz"));
	else if  (now==dateTimeEdit) selectLabel->setText(trUtf8("Ustaw"));
}

void InspectionScreen::keyPressEvent( QKeyEvent * event )
{
	//ESCAPE SWITCH
        if (event->key() == Qt::Key_Escape) {
		close();
        }
        if ((event->key() == Qt::Key_Play)||(event->key() == Qt::Key_F2)) {
		if (inspectionDateEdit->hasFocus()) {
			system->setValue("inspection",inspectionDateEdit->date());
			system->sync();
			QProcess sync;
			sync.start("/bin/sync");
			sync.waitForFinished();
		}
		if (dateTimeEdit) {
			RealTimeClock rtc;
			rtc.set(dateTimeEdit->dateTime());
		}
	}
}
