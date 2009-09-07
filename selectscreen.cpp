//
// C++ Implementation: SelectScreen
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
#include <QEvent>
#include <QSettings>
#include <QString>
#include <QTime>
#include "selectscreen.h"

SelectScreen::SelectScreen(QWidget *parent, Qt::WFlags f)
    :QMainWindow(parent, f)
{
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);

	part =1;
	size =1;

	setupUi(this);
	humanView->setScript("../resources/humanView.qjs" );
	settings = new QSettings("/home/kosiu/kriosan.ini", QSettings::IniFormat);
	updateStatus();
}

void SelectScreen::showSelectScreen(){
	show();updateStatus();
}
void SelectScreen::updateStatus(){
	const int powerDefault[] = {
		//small         //medium        //large	
		80,		90,		100,		//shoulder
		50,		70,		70,		//elbow
		50,		50,		70,		//wirst
		80,		100,		100,		//hip
		50,		70,		90,		//knee
		50,		70,		70,		//ankle
	};
	const QTime timeDefault[] = {
		//small         //medium        //large	
		QTime(0,3,0),	QTime(0,5,0),	QTime(0,7,0),	//shoulder
		QTime(0,2,0),	QTime(0,2,0),	QTime(0,3,30),	//elbow
		QTime(0,1,30),	QTime(0,2,0),	QTime(0,2,30),	//wirst
		QTime(0,3,0),	QTime(0,4,0),	QTime(0,6,0),	//hip
		QTime(0,3,0),	QTime(0,3,0),	QTime(0,4,0),	//knee
		QTime(0,2,0),	QTime(0,2,30),	QTime(0,3,0)	//ankle
	};
	int operationNumer = (part - 1) * 3 + size;

	backLabel->setText(QString(trUtf8("Wyjście")));
	QString powerStr=QString("operation_power_%1").arg(operationNumer);
	QString timeStr =QString("operation_time_%1" ).arg(operationNumer);

	power = settings->value(powerStr, powerDefault[operationNumer-1]).toInt();
	time = settings->value(timeStr, timeDefault[operationNumer-1]).toTime();
	qDebug("part: %d, size: %d, zabieg: %d",part,size,operationNumer);

	QString textLabel=QString(trUtf8("Inten.: %1%, czas: %2"))
	.arg(power).arg(time.toString("mm:ss"));
	statuslabel->setText(textLabel);
}

void SelectScreen::keyPressEvent( QKeyEvent * event )
{
	//ESCAPE SWITCH
        if (event->key() == Qt::Key_Escape) {
		emit(SendShowMenuScreen(mainView));hide();
        } else
        if (event->key() == Qt::Key_Tab) {
		part++;if (part>6) part = 1;
		humanView->setPName("part");
		humanView->setPValue(part);
		updateStatus();
        } else
        if (event->key() == Qt::Key_Backtab) {
		part--;if (part<1) part = 6;
		humanView->setPName("part");
		humanView->setPValue(part);
		updateStatus();
        } else
        if (event->key() == Qt::Key_Play) {
		OperationScreenType operationType;
		operationType.operationType = part;
		operationType.power = power;
		operationType.time = &time;
		emit(SendShowOperationScreen(operationType));
	}
}

bool SelectScreen::event(QEvent *event){
	if (event->type() == QEvent::ShortcutOverride) {
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		if (ke->key() == Qt::Key_Up){
		    size++;if (size>3) size = 1;
		    humanView->setPName("size");
		    humanView->setPValue(size);
		    updateStatus();

		    return true;
		}
		if (ke->key() == Qt::Key_Down) {
		    size--;if (size<1) size = 3;
		    humanView->setPName("size");
		    humanView->setPValue(size);
		    updateStatus();


		    return true;
		}
	}
	return QWidget::event(event);
}
