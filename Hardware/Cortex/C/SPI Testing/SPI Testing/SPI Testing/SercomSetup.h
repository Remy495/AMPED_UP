/*
 * IncFile1.h
 *
 * Created: 3/12/2021 11:28:36 AM
 *  Author: hanle
 */ 

#include "SercomConfig.h"
#ifndef SERCOMSETUP_H_
#define SERCOMSETUP_H_
	SercomConfig SPIMaster;
	SercomSetIndex(SPIMaster,SERCOM_0);
	spi_master_setup(SPIMaster);
	




#endif /* INCFILE1_H_ */