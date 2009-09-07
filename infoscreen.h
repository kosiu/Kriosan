//
// C++ Interface: InfoScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include "ui_infoscreen.h"
class QMainWindow;

class InfoScreen: public QMainWindow, public Ui::infoScreen
{
Q_OBJECT

public:
	InfoScreen(QWidget * parent = 0, Qt::WFlags f = 0);

protected:
	virtual void keyPressEvent( QKeyEvent * event );
};

#endif
