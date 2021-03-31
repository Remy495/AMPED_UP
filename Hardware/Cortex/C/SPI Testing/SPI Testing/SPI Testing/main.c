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
const uint8_t required[9][5] ={{0xEC,0x01,0x01,0x00,0xC2},{0x90,0x00,0x06,0x1F,0x0A},{0x91,0x00,0x00,0x00,0x0A},{0x80,0x00,0x00,0x00,0x03},{0x93,0x00,0x00,0x00,0x00},{0x6F,0x00,0x00,0x00,0x00}};
int QEM[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
int target = 0;
int buffer = 0;
int target2 = 10000;
int threshold = 25;
uint8_t current=0;
uint8_t previous=0;
int count=0;
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
	setPin(CFG5,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG6,OUTPUT,NORMAL,PULL_DOWN);
	setPin(STEP,OUTPUT,NORMAL,PULL_DOWN);
	setPin(DIRPIN,OUTPUT,NORMAL,PULL_DOWN);
	writePin(CFG3,true);
	writePin(CFG4,false);
	writePin(CFG5,false);
	writePin(CFG6,false);
	writePin(DIRPIN,direction);
	spi_master_init();
	writePin(SPI_MODE,true);
	for(int j = 0; j<5; j++){
		spi_send_config(required[j], recv[j]);
	}
	return;
}
void EIC_Handler(void){
	if(EIC->INTFLAG.reg & (EIC_INTFLAG_EXTINT4)|(EIC->INTFLAG.reg & EIC_INTFLAG_EXTINT5)){
		uint32_t in = PORT->Group[0].IN.reg;
		uint8_t bit1 = (bool)(in & PA04.bitmask); 
		uint8_t bit2 = (bool)(in & PA05.bitmask);
		previous=current;
		current=2*bit1+bit2;
		count+=QEM[previous*4+current];
		if(count > 20000){count--;}
		if(count < -1000){count++;}
		EIC->INTFLAG.reg |= EIC_INTFLAG_EXTINT4| EIC_INTFLAG_EXTINT5;
	}
	}


int main(void)
{
	count=0;
	direction=false;
	SYSCTRL->OSC8M.bit.PRESC = 0x0;//Making core clock 8MHz
	initPins();
	int counts=0;
	int change = 5000;
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_EIC | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA04A_EIC_EXTINT4)|PORT_WRCONFIG_PINMASK((uint16_t)PA04.bitmask);
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA05A_EIC_EXTINT5)|PORT_WRCONFIG_PINMASK((uint16_t)PA05.bitmask);
	EIC->EVCTRL.reg |=EIC_EVCTRL_EXTINTEO4 | EIC_EVCTRL_EXTINTEO5;
	EIC->CONFIG[0].reg |= EIC_CONFIG_FILTEN4|EIC_CONFIG_SENSE4(0x3)|EIC_CONFIG_FILTEN5|EIC_CONFIG_SENSE5(0x3);
	EIC->INTENSET.reg|= EIC_INTENSET_EXTINT4|EIC_INTENSET_EXTINT5;
	EIC->CTRL.reg |=  EIC_CTRL_ENABLE;
	NVIC_SetPriority(EIC_IRQn,0);
	NVIC_EnableIRQ(EIC_IRQn);
	__enable_irq();
	unsigned long input=0;
	setPin(LED, OUTPUT, NORMAL, PULL_DOWN);
	writePin(LED,false);
	initRTC();
	standalone_mode();
	//spi_mode();
	bool toggle=true;
	bool isStepping = true;
	//previous = (readPin(&PA04))*2+ (readPin(&PA05));
	while (1)
    {	
		if(isStepping){
			delay_us(10);
			writePin(STEP,toggle);
			toggle=!toggle;
		}
		else
		{
			buffer=target;
			target=target2;
			target2=buffer;
			isStepping=true;
		}
		if(count > target+threshold){writePin(DIRPIN,false);}
		else if(count < target-threshold){writePin(DIRPIN,true);}
		if(target-threshold < count){
			isStepping=false;
			}
		if(count < target + threshold){
			isStepping=false;
			}
		if(count >target+ threshold){isStepping=true;}
		if(count<target-threshold){isStepping=true;}
			/*
		writePin(STEP,toggle);
		toggle = !toggle;
		counts++;
		delay_us(100);
		if(counts==change){
			writePin(DIRPIN,direction);
			spi_send_config(required[5],recv[5]);
			spi_send_config(required[6],recv[6]);
			direction=!direction;
			counts=0;
		}*/
    }
}

