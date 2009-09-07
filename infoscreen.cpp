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

InfoScreen::InfoScreen(QWidget *parent, Qt::WFlags f)
    :QMainWindow(parent, f)
{
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
	
	setupUi(this);
	backLabel->setText(QString(trUtf8("Wyjście")));
	//QTextDocument infoText();
	textBrowser->setSource(QUrl("../resources/publicinfo.html"));
	titleBrowser->setSource(QUrl("../resources/kriosystem.html"));

	if(textBrowser->find("$DATE$")){
		textBrowser->cut();
		textBrowser->insertPlainText(QDate(2008,5,9).toString());
	}
}

void InfoScreen::keyPressEvent( QKeyEvent * event )
{
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

