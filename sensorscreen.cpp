//
// C++ Implementation: SensorScreen
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QProcess>
#include <QKeyEvent>
#include <QString>
#include <QSettings>
#include "sensorscreen.h"
#include "adcdevice.h"


SensorScreen::SensorScreen(ADCDevice* adc_readArg, Buzzer* buzzer,QWidget *parent, Qt::WFlags f)
    :QMainWindow(parent, f)
{
	adc_read=adc_readArg;
	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);

	setupUi(this);
	adc_read->startConversions(3 , 200);
	connect(adc_read, SIGNAL(channel3(int)), this, SLOT(levelVoltage(int)));
	connect(adc_read, SIGNAL(channel2(int)), this, SLOT(temperatureVoltage(int)));
	connect(adc_read, SIGNAL(channel1(int)), this, SLOT(keyVoltage(int)));

	system = new QSettings("/home/kosiu/system.ini", QSettings::IniFormat);

	emptyBottleEdit->setValue(system->value("Bottle_Min", 0.0).toDouble());
	fullBottleEdit->setValue(system->value("Bottle_Max", 2.04).toDouble());

	serviceKeyMinEdit->setValue(system->value("Key_Min", 0.4).toDouble());
	serviceKeyMaxEdit->setValue(system->value("Key_Max", 2.04).toDouble());

	temperatureThresholdEdit->setValue(system->value("Temperature", 77.4).toDouble());

}

void SensorScreen::levelVoltage(int value)
{
	value = (value / 16); //in mV	
	QString text = QString(trUtf8(" Poziomu azotu (napięcie): %1V")).arg(value/1e3,0,'G',3);
	levelVoltageLabel->setText(text);

	int min = emptyBottleEdit->value() * 1000.0;
	int max =  fullBottleEdit->value() * 1000.0;
	value = (100 * (value - min)) / (max - min);
	if (value>100) value = 100; if (value<0) value = 0;
	text = QString(trUtf8(" Poziom azotu: %1% ")).arg(value);
	levellabel->setText(text);
}

void SensorScreen::temperatureVoltage(int voltage)
{
	//float temp = 2341.2 * voltage / 16000.0 / (14.19 - voltage/16000.0) - 29.5808;
	//celc = 2.3412 * R - 243.4192
	float temp = 0.01067 * voltage + 34.0886;
	QString text = QString(trUtf8(" Termometr: %1K")).arg(temp,0,'G',3);
	temperatureLabel->setText(text);
}

void SensorScreen::keyVoltage(int value)
{
	float voltage = (value /16) / 1000.0;
	
	QString text = QString(trUtf8(" Klucz serwisowy: %1V")).arg(voltage,0,'G',3);
	serviceKeyLabel->setText(text);
}

void SensorScreen::keyPressEvent( QKeyEvent * event )
{
	//ESCAPE SWITCH
        if (event->key() == Qt::Key_Escape) {
		adc_read->startConversions(2 , 250);
		close();
        }
	//SELECT SWITCH
	if (event->key() == Qt::Key_Play) {
		system->setValue("Bottle_Min",emptyBottleEdit->value());
		system->setValue("Bottle_Max",fullBottleEdit->value());

		system->setValue("Key_Min",serviceKeyMinEdit->value());
		system->setValue("Key_Max",serviceKeyMaxEdit->value());

		system->setValue("Temperature",temperatureThresholdEdit->value());
		system->sync();
		QProcess sync;
		sync.start("/bin/sync");
		sync.waitForFinished();
	}
}

	