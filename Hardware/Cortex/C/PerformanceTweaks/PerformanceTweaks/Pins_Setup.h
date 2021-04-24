/*
 * Pins_Setup.h
 *
 * Created: 3/10/2021 7:30:47 PM
 *  Author: hanley
 */ 

#include "Pins.h"
#ifndef PINS_SETUP_H_
#define PINS_SETUP_H_
struct pin PA00;
struct pin PA01;
struct pin PA02;
struct pin PA03;
struct pin PA04;
struct pin PA05;
struct pin PA06;
struct pin PA07;
struct pin PA08;
struct pin PA09;
struct pin PA10;
struct pin PA11;
struct pin PA14;
struct pin PA15;
struct pin PA16;
struct pin PA17;
struct pin PA18;
struct pin PA19;
struct pin PA22;
struct pin PA23;
struct pin PA24;
struct pin PA25;
struct pin PA27;
struct pin PA28;
struct pin PA29;
struct pin PA30;
struct pin PA31;
void initPins(){
enum port Port=PORTA;
setPinPort(&PA00,Port);
setPinPort(&PA01,Port);
setPinPort(&PA02,Port);
setPinPort(&PA03,Port);
setPinPort(&PA04,Port);
setPinPort(&PA05,Port);
setPinPort(&PA06,Port);
setPinPort(&PA07,Port);
setPinPort(&PA08,Port);
setPinPort(&PA09,Port);
setPinPort(&PA10,Port);
setPinPort(&PA11,Port);
setPinPort(&PA14,Port);
setPinPort(&PA15,Port);
setPinPort(&PA16,Port);
setPinPort(&PA17,Port);
setPinPort(&PA18,Port);
setPinPort(&PA19,Port);
setPinPort(&PA22,Port);
setPinPort(&PA23,Port);
setPinPort(&PA24,Port);
setPinPort(&PA25,Port);
setPinPort(&PA27,Port);
setPinPort(&PA28,Port);
setPinPort(&PA29,Port);
setPinPort(&PA30,Port);
setPinPort(&PA31,Port);
enum num Num = P00;
setPinNum(&PA00,Num);
Num=P01;
setPinNum(&PA01,Num);
Num=P02;
setPinNum(&PA02,Num);
Num=P03;
setPinNum(&PA03,Num);
Num=P04;
setPinNum(&PA04,Num);
Num=P05;
setPinNum(&PA05,Num);
Num=P06;
setPinNum(&PA06,Num);
Num=P07;
setPinNum(&PA07,Num);
Num=P08;
setPinNum(&PA08,Num);
Num=P09;
setPinNum(&PA09,Num);
Num=P10;
setPinNum(&PA10,Num);
Num=P11;
setPinNum(&PA11,Num);
Num=P14;
setPinNum(&PA14,Num);
Num=P15;
setPinNum(&PA15,Num);
Num=P16;
setPinNum(&PA16,Num);
Num=P17;
setPinNum(&PA17,Num);
Num=P18;
setPinNum(&PA18,Num);
Num=P19;
setPinNum(&PA19,Num);
Num=P22;
setPinNum(&PA22,Num);
Num=P23;
setPinNum(&PA23,Num);
Num=P24;
setPinNum(&PA24,Num);
Num=P25;
setPinNum(&PA25,Num);
Num=P27;
setPinNum(&PA27,Num);
Num=P28;
setPinNum(&PA28,Num);
Num=P29;
setPinNum(&PA29,Num);
Num=P30;
setPinNum(&PA30,Num);
Num=P31;
setPinNum(&PA31,Num);
}
#endif /* PINS_SETUP_H_ */