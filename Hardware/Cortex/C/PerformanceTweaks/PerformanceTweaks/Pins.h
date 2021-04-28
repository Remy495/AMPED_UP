/*
 * IncFile1.h
 *
 * Created: 3/9/2021 6:46:27 PM
 *  Author: hanle
 */ 
//All IO Register Addresses
#define DIR 0x00
//#define DIRCLR 0x04
//#define DIRSET 0x08
//#define DIRTGL 0x0C
//#define OUT 0x10
//#define OUTCLR 0x14
//#define OUTSET 0x18
//#define OUTTGL 0x1C
//#define IN 0x20
//#define CRTL 0x24
//#define WRCONFIG 0x28
#define BITSINWORD 32
#define BITSINHALFWORD 16
//Our project only uses Port A pins, and therefore we will be only utiizng those
#include "stdbool.h"
#ifndef PINS_H_
#define PINS_H_

//each pin can be accessed through a number of ports, each with their own set of configuration registers. For all of this project, we will be using and configuring using port A.
enum num{P00=0,P01=1,P02=2,P03=3,P04=4,P05=5,P06=6,P07=7,P08=8,P09=9,P10=10,P11=11,P12=12,P13=13,P14=14,P15=15,P16=16,P17=17,P18=18,P19=19,P20=20,P21=21,P22=22,P23=23,P24=24,P25=25,P26=26,P27=27,P28=28,P29=29,P30=30,P31=31};
enum port{PORTA,PORTB,PORTC}port;
//Setting the pin output drive strength
enum pull_en{NORMAL,HIGH};
//resistor Pull States, can be configured for both pull up operation and pull down
enum input_res_pul{NONE,PULL_UP,PULL_DOWN};	
//Each Pin can be multiplexed to have different modes of operation. For default I/O, this value is 0 for INPUT, and 1 for OUTPUT
enum pin_mode {
	EXTERNAL_INTERUPT_E = PORT_PMUX_PMUXE_A_Val,
	EXTERNAL_INTERUPT_O = PORT_PMUX_PMUXO_A_Val,
	DAC_OR_ADC_E = PORT_PMUX_PMUXE_B_Val,
	DAC_OR_ADC_O = PORT_PMUX_PMUXO_B_Val,
	SERCOM_E = PORT_PMUX_PMUXE_C_Val,
	SERCOM_O = PORT_PMUX_PMUXO_C_Val,
	SERCOM_ALT_E = PORT_PMUX_PMUXE_D_Val,
	SERCOM_ALT_O = PORT_PMUX_PMUXO_D_Val,
	TIMER_COUNTER_E = PORT_PMUX_PMUXE_E_Val,
	TIMER_COUNTER_O = PORT_PMUX_PMUXO_E_Val,
	TIMER_COUNTER_CONTROL_E = PORT_PMUX_PMUXE_F_Val,
	TIMER_COUNTER_CONTROL_O = PORT_PMUX_PMUXO_F_Val,
	COM_E = PORT_PMUX_PMUXE_G_Val,
	COM_O = PORT_PMUX_PMUXO_G_Val,
	GENERIC_CLOCK_E = PORT_PMUX_PMUXE_H_Val,
	GENERIC_CLOCK_O = PORT_PMUX_PMUXO_H_Val,
	INPUT,
	OUTPUT
	};
struct pin{
uint32_t bitmask;
uint32_t PMUXSEL; 
uint32_t wrConfigVal;
bool isPeripheralCont;
enum port _port;
enum pin_mode _pin_mode;
enum pull_en _pull_en;
enum input_res_pul _input_res_pul;
enum num _num;
};

void setPinPort(struct pin* Pin, enum port Port){
	Pin->_port=Port;
	Pin->isPeripheralCont=false;
	return;
}
void setPinNum(struct pin* Pin, enum num Num){
	Pin->_num=Num;
	Pin->bitmask=0;
	Pin->bitmask = 1UL<< Num;
	return;
}
void setPinDrive(struct pin* Pin, enum pull_en Pull_en){
	Pin->_pull_en=Pull_en;
	return;
}
void setPinResPull(struct pin* Pin, enum input_res_pul Pull){
	Pin->_input_res_pul=Pull;
	return;
}
void setPinDir(struct pin* Pin, enum pin_mode dir){//Set after Port, Num, and pull up/down or
	Pin->_pin_mode=dir;
	if(dir == OUTPUT){
		PORT->Group[Pin->_port].DIRSET.reg = Pin->bitmask;
		Pin->isPeripheralCont = false;
	}
	else if(dir==INPUT){
		Pin->isPeripheralCont = false;
		PORT->Group[Pin->_port].DIRCLR.reg = Pin->bitmask;
		if(Pin->_input_res_pul == PULL_UP){
			PORT->Group[Pin->_num].OUTSET.reg = Pin->bitmask;
		}
		else if(Pin->_input_res_pul==PULL_DOWN){
			PORT->Group[Pin->_num].OUTCLR.reg = Pin->bitmask;
		}
	}
	else{Pin->isPeripheralCont = true;}
}
void PinCFG(struct pin* Pin, enum pin_mode dir, enum pull_en Pull_en, enum input_res_pul Pull){
	setPinDrive(Pin,Pull_en);
	setPinResPull(Pin,Pull);
	setPinDir(Pin,dir);
	Pin->wrConfigVal |= PORT_WRCONFIG_WRPINCFG;
	uint16_t pinBitmaskHalf = 1;
	if(Pin->_num < BITSINHALFWORD){
		pinBitmaskHalf <<= Pin->_num;
	}
	else{
		Pin->wrConfigVal |= PORT_WRCONFIG_HWSEL;
		pinBitmaskHalf <<= (Pin->_num - BITSINHALFWORD);
	}
	Pin->wrConfigVal |= PORT_WRCONFIG_PINMASK(pinBitmaskHalf);
	if(Pin->_pin_mode == OUTPUT &&Pin->_pull_en == HIGH){
		Pin->wrConfigVal |= PORT_WRCONFIG_DRVSTR;
	}
	if(Pin->_pin_mode == INPUT){
		Pin->wrConfigVal |= PORT_WRCONFIG_INEN;
	}
	if(Pin->_pin_mode == INPUT && Pin->_input_res_pul == NONE){
		Pin->wrConfigVal |= PORT_WRCONFIG_PULLEN;
	}
	if(Pin->isPeripheralCont){
		Pin->wrConfigVal |= PORT_WRCONFIG_PMUX((uint8_t)Pin->_pin_mode);
		Pin->wrConfigVal|= PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN;
		
	}
}

void setPin(struct pin* Pin, enum pin_mode dir, enum pull_en Pull_en, enum input_res_pul Pull ){
	PinCFG(Pin, dir, Pull_en, Pull);
	PORT->Group[Pin->_port].WRCONFIG.reg = Pin->wrConfigVal;
}


void writePin(struct pin* Pin, bool val){
	if(val){
		PORT->Group[Pin->_port].OUTSET.reg = Pin->bitmask;
	}
	if(!val){
		PORT->Group[Pin->_port].OUTCLR.reg = Pin->bitmask;
	}
}
bool readPin(struct pin* Pin){
	unsigned long result = PORT->Group[Pin->_port].IN.reg;
	return result & Pin->bitmask;
}
#endif /* INCFILE1_H_ */