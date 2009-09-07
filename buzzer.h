//
// C++ Interface: Buzzer
//
// Description:
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BUZZER_H
#define BUZZER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
class QFile;

class Buzzer : public QThread
{
Q_OBJECT

public:
	Buzzer(QObject *parent =0);
	~Buzzer();

public slots:
	void beep(int msec);

private:
	QWaitCondition cond;
	QMutex mutex;
	bool quit;
	int msec;

protected:
     void run();

};

#endif // BUZZER_H
