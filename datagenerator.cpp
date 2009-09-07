//
// C++ Implementation: DataGenerator
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2007, 2008, 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
using namespace std;
#include <QTimer>
#include "datagenerator.h"
#include "i2c-dev.h"

DataGenerator::DataGenerator(QObject* parent) : QObject(parent)
{
#ifdef __arm__
	int adapter_nr = 0; /* probably dynamically determined */
	char filename[20];
	int addr = 75;      /* The I2C address */
	char buf[4];
	
	/* Open adapter */
	sprintf(filename,"/dev/i2c-%d",adapter_nr);
	if ((file = open(filename,O_RDWR)) < 0) {
		qDebug("Nie mogę oteorzyć szyny");
	}
	
	/* open sensor */
	if (ioctl(file,I2C_SLAVE,addr) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		qDebug("Nie mogę oteorzyć adresu 73");
	}
	
	buf[0] = 0x80;
	if ( write(file,buf,1) != 1) {
		/* ERROR HANDLING: i2c transaction failed */
		qDebug("Nie mogę zapisać");
	}
#endif

	interval = new QTimer(this);
	connect(interval, SIGNAL(timeout()), this, SLOT(sendPoint()));
	interval->start(100);
}

void DataGenerator::sendPoint()
{
	interval->start(500);

#ifdef __arm__
	int y;
	char buf[4];

	if ( read(file,buf,3) != 3) {
		qDebug("Nie mogę czytać.");
		/* ERROR HANDLING: i2c transaction failed */
	} else {
		y = ( buf[0]*256+buf[1] )/6;
		emit( newPoint(qRound(y)) );
	}
#else
		emit( newPoint(rand()%101) );
#endif
}
