//
// C++ Interface: DataGenerator
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2007, 2008, 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include<QObject>
class QTimer;
class QTextStream;


class DataGenerator : public QObject
{
    Q_OBJECT

    public:
        DataGenerator(QObject* parent = 0);

    signals:
        void newPoint(int point);

    private:
        QTimer* interval;
	int file;

    private slots:
        void sendPoint();


};

#endif
