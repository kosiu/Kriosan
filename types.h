//
// C++ Interface: CLASS_NAME
//
// Description: 
//
//
// Author: Jacek Kosek <jacek.kosek@pwr.wroc.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
// 
//
#ifndef TYPES_H
#define TYPES_H
#include "types.h"
#include <QTime>

//Path to system settings ini file
const char SysConfigFileName[] = "/media/gum/home/kosiu/system.ini";

//Path to user settings ini file
const char UserConfigFileName[] ="/media/gum/home/kosiu/kriosan.ini";


//MenuScreenType has tree different views
enum MenuScreenType { mainView=0, optionView=1, languageView=2, /*operationView=3*/ };

//OperationScreenType is activated with this struct
struct OperationScreenType {
	int operationType;	// 0 - continues operation
				// 1 - shoulder
				// 2 - elbow
				// 3 - wirst
				// 4 - hip
				// 5 - knee
				// 6 - ankle
	int power;		// power value in %
	QTime* time;		// time of operation
};

#endif
