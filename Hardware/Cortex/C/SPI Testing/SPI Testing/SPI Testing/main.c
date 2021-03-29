/*
 * SPI Testing.c
 *
 * Created: 3/3/2021 1:14:01 PM
 * Author : hanleya
 */ 


#include "sam.h"
#include "Pins_Setup.h"
#include "Timing.h"
#include "TempSPI.h"
//#include "SercomSetup.h"
//#include "spi_control.h"
#define LED &PA25
#define IFA &PA04
#define IFB &PA05
#define IFC &PA06
#define CFG0 &PA08
#define CFG1 &PA11
#define CFG2 &PA09
#define CFG3 &PA10
#define CFG4 &PA18
#define CFG5 &PA19
#define CFG6 &PA16
#define SPI_MODE &PA17
#define STEP &PA14
#define DIRPIN &PA15
volatile uint8_t recv[9][5] = {{0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00}};
const uint8_t required[9][5] ={{0xEC,0x04,0x01,0x00,0xC2},{0x90,0x00,0x06,0x1F,0x0A},{0x91,0x00,0x00,0x00,0x0A},{0x80,0x00,0x00,0x00,0x03},{0x93,0x00,0x00,0x00,0x00}};
//volatile int counts;
bool direction;

void standalone_mode(){
	setPin(CFG0,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG1,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG2,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG3,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG4,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG5,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG6,OUTPUT,NORMAL,PULL_DOWN);
	setPin(STEP,OUTPUT,NORMAL,PULL_DOWN);
	setPin(DIRPIN,OUTPUT,NORMAL,PULL_DOWN);
	setPin(SPI_MODE,OUTPUT,NORMAL,PULL_DOWN);
	//setPin(IFA,INPUT,NORMAL,PULL_DOWN);
	//setPin(IFB,INPUT,NORMAL,PULL_DOWN);
	writePin(SPI_MODE,false);
	writePin(CFG0,false);//MISO =PA08
	writePin(CFG1,true);//MOSI = PA11
	writePin(CFG2,true);//SCK = PA09
	writePin(CFG3,true);//SS= PA10
	writePin(CFG4,false);
	writePin(CFG5,true);
	writePin(CFG6,false);
	writePin(DIRPIN,direction);
	writePin(STEP,false);
	return;
}
void spi_mode(){

	setPin(CFG3,OUTPUT,NORMAL,PULL_DOWN);
	setPin(SPI_MODE,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG4,OUTPUT,NORMAL,PULL_DOWN);
	//setPin(CFG5,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG6,OUTPUT,NORMAL,PULL_DOWN);
	setPin(STEP,OUTPUT,NORMAL,PULL_DOWN);
	setPin(DIRPIN,OUTPUT,NORMAL,PULL_DOWN);
	writePin(CFG3,true);
	writePin(CFG4,false);
	writePin(CFG6,false);
	writePin(DIRPIN,direction);
	spi_master_init();
	writePin(SPI_MODE,true);
	for(int j = 0; j<5; j++){
		spi_send_config(required[j], recv[j]);
	}
	return;
}
int main(void)
{
	direction=false;
	SYSCTRL->OSC8M.bit.PRESC = 0x0;//Making core clock 8MHz
	initPins();
	int counts=0;
	int change = 5000;
	setPin(LED, OUTPUT, NORMAL, PULL_DOWN);
	writePin(LED,true);
	initRTC();
	standalone_mode();
	//spi_mode();
	bool toggle=true;
	while (1)
    {

		delay_us(50);
		//writePin(LED,toggle);
		writePin(STEP,toggle);
		toggle=!toggle;
		counts++;
		if(counts==change){
			writePin(DIRPIN,direction);
			direction=!direction;
			counts=0;
		}

    }
}

