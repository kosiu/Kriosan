//
// C++ Implementation: RealTimeClock
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "realtimeclock.h"
#include "i2cdevice.h"
#include <QDateTime>
#include <QtDebug>

RealTimeClock::RealTimeClock(QObject *parent)
 : QObject(parent)
{
}

const QDateTime RealTimeClock::get()
{
	char bufor[16]={0};

	I2CDevice *i2cDevice;

	i2cDevice = new I2CDevice(104);
	i2cDevice->writeI2C(bufor,1);
	i2cDevice->readI2C(bufor, 8);

	delete i2cDevice;

	qDebug("read: %d %d %d %d %d %d %d %d",
		bufor[0],bufor[1],bufor[2],bufor[3],bufor[4],bufor[5],bufor[6],bufor[7]);

	const char  lowBCDmask = 0x0f;
	const char highBCDmask = 0xf0;

	int minute = 10 * ( (bufor[1] & highBCDmask)/16 ) + ( bufor[1] & lowBCDmask );
	int hour   = 10 * ( (bufor[2] & highBCDmask)/16 ) + ( bufor[2] & lowBCDmask );

	int day    = 10 * ( (bufor[4] & highBCDmask)/16 ) + ( bufor[4] & lowBCDmask );
	int month  = 10 * ( (bufor[5] & highBCDmask)/16 ) + ( bufor[5] & lowBCDmask );
	int year   = 10 * ( (bufor[6] & highBCDmask)/16 ) + ( bufor[6] & lowBCDmask ) + 2000;


	QDate date(year,month,day);
	QTime time(hour, minute);
	qDebug()<<hour<<":"<<minute<<day<<"-"<<month<<"-"<<year;
	qDebug()<<time.toString()<<date.toString();
#ifdef __arm__
	QDateTime dateTime(date,time);
#else
	QDateTime dateTime = QDateTime::currentDateTime();
#endif
	return dateTime;
}

void RealTimeClock::set(QDateTime dateTime)
{
	QDate date = dateTime.date();
	QTime time = dateTime.time();

	int minute = time.minute();
	int hour   = time.hour();

	int day    = date.day();
	int month  = date.month();
	int year   = date.year() - 2000;

	char buf[10] = {0};

	buf[2] = (minute / 10) * 16 + minute % 10;
	buf[3] = (hour   / 10) * 16 + hour   % 10;

	buf[4] = 1;//DAY OF THE WEEK

	buf[5] = (day    / 10) * 16 + day    % 10;
	buf[6] = (month  / 10) * 16 + month  % 10;
	buf[7] = (year   / 10) * 16 + year   % 10;

	qDebug("write: %d %d %d %d %d %d %d %d %d",
		buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8]);
	
	I2CDevice *i2cDevice;

	i2cDevice = new I2CDevice(104);
	i2cDevice->writeI2C(buf, 9);
	delete i2cDevice;
}

