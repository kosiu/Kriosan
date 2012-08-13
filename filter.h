//
// C++ Interface: Filter
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009, 2012
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILTER_H
#define FILTER_H

#include <QObject>
class ADCDevice;

/**
This class recive signals from adc converter, then convert for more interesting signals, filter them and send futher.
*/
class Filter : public QObject
{
Q_OBJECT
public:
    Filter(QObject *parent =0);
    ~Filter();
    static ADCDevice adc_read;
    float mVoltage;
    float mLevel;
    float mTemperature;
    bool brokenTemperatureSensor;

public slots:
    void channel1(int value);
    void channel2(int value);
    void channel3(int value);

signals:
    void keyValue(float voltage);
    void levelValue(float level);
    void tempValue(float temp);
private:
    float computeFilter(int &value, int bufor[], int &index, bool& first_iteration, int lenght);
};

#endif
