//
// C++ Interface: Filter
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILTER_H
#define FILTER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

/**
This class recive signals from adc converter, then convert for more interesting signals, filter them and send futher.
*/
class Filter : public QThread
{
Q_OBJECT
public:
    Filter(QObject *parent =0);
    ~Filter();

public slots:
    void channel1(int value);
    void channel2(int value);
    void channel3(int value);

signals:
    void keyValue(float voltage);
    void levelValue(float level);
    void temperatureValue(float temperature);

private:
	QWaitCondition cond;
	QMutex mutex;
	bool quit;
	int val; //value of signal recivied
	int channel; //wich channel

protected:
     void run();

};

#endif