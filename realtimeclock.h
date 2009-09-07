//
// C++ Interface: RealTimeClock
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include <qobject.h>
class QDateTime;
class I2CDevice;

class RealTimeClock : public QObject
{
Q_OBJECT
public:
	RealTimeClock(QObject *parent = 0);
	const QDateTime get();
	void set(QDateTime date);

private:
	static I2CDevice i2cDevice;
};

#endif
