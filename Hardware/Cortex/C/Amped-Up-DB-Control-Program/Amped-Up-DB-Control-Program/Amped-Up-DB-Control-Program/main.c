/*
 * SPI Testing.c
 *
 * Created: 3/3/2021 1:14:01 PM
 * Author : hanleya
 */ 


#include "math.h"
#include "sam.h"
#include "Pins_Setup.h"
#include "Timing.h"
#include "TempSPI.h"
#include "MainBoardMessager.h"

//#include "SercomSetup.h"
//#include "spi_control.h"
#define INTER &PA25
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
const uint8_t required[9][5] ={{0xEC,0x00,0x01,0x00,0xC3},{0x90,0x00,0x06,0x1F,0x0A},{0x80,0x00,0x00,0x00,0x03},{0x93,0x00,0x00,0x00,0x00},{0xF0,0x00,0x04,0x01,0xC8},{0x6F,0x00,0x00,0x00,0x00}};
int QEM[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
uint8_t current=0;
uint8_t previous=0;
int stepsSinceChange=0;
float target = 0.5;
int threshold=2000;
volatile int count=0;
int counts=0;
volatile int stepsTotal=0;
volatile int steps1;
volatile int steps2;
volatile int encTot;
int8_t identifier;
int8_t prev3;
int8_t prev2;
int8_t prevDirection;
int8_t currentDirection;
int16_t floorcount;
int16_t prevcount;
uint32_t in;
bool dip0;
bool dip1;
bool dip2;
bool isStepping=true;
bool toggle=true;
bool dirchange=false;
bool isStalled=false;
bool setup=true;
bool direction;
bool isGrabbed=false;
int countdelta;

void standalone_mode(){
	setPin(CFG0,OUTPUT,NORMAL,PULL_DOWN);
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PULLEN|PORT_WRCONFIG_PINMASK((uint16_t)PA00.bitmask);//dip0
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PULLEN|PORT_WRCONFIG_PINMASK((uint16_t)PA01.bitmask);//dip1
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PULLEN|PORT_WRCONFIG_PINMASK((uint16_t)PA02.bitmask);//dip2
	PORT->Group[0].OUT.reg |= PORT_WRCONFIG_PINMASK((uint16_t)PA00.bitmask)|PORT_WRCONFIG_PINMASK((uint16_t)PA01.bitmask)|PORT_WRCONFIG_PINMASK((uint16_t)PA02.bitmask);
	setPin(CFG1,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG2,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG3,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG4,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG5,OUTPUT,NORMAL,PULL_DOWN);
	setPin(CFG6,OUTPUT,NORMAL,PULL_DOWN);
	setPin(STEP,OUTPUT,NORMAL,PULL_DOWN);
	setPin(DIRPIN,OUTPUT,NORMAL,PULL_DOWN);
	setPin(SPI_MODE,OUTPUT,NORMAL,PULL_DOWN);
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
	dip0 = readPin(&PA00);
	dip1 = readPin(&PA01);
	dip2 = readPin(&PA02);
	identifier = (int)dip0|(int)dip1<<1|(int)dip2<<2;
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

#define STEP_BUFFER_SIZE 5

//volatile int8_t directionFilter[STEP_BUFFER_SIZE] = {0};
//volatile int8_t currentDirectionIndex = 0;

void EIC_Handler(void){
		if((EIC->INTFLAG.reg & EIC_INTFLAG_EXTINT5)||(EIC->INTFLAG.reg & EIC_INTFLAG_EXTINT4)){
		uint32_t in = PORT->Group[0].IN.reg;
		uint8_t bit1 = (bool)(in & PA04.bitmask); 
		uint8_t bit2 = (bool)(in & PA05.bitmask);
		previous=current;
		current=2*bit1+bit2;
		prev3=prev2;
		prev2=prevDirection;
		prevDirection=currentDirection;
		currentDirection = QEM[previous*4+current];
		count+=currentDirection;
		countdelta++;
		if(setup && stepsSinceChange>50 && ((prev2 == -1 && prevDirection==-1 && currentDirection == -1&&direction)|| (prev2 == 1 && prevDirection==1 && currentDirection == 1 && !direction)) && !isStalled){
			writePin(&PA25,true);
			isStalled=true;
		}
		if(!isStepping && countdelta >= threshold){
			isGrabbed=true;
		}
		EIC->INTFLAG.reg |= EIC_INTFLAG_EXTINT4| EIC_INTFLAG_EXTINT5;
	}
}
void EIC_setup(void){
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_EIC | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA05A_EIC_EXTINT5)|PORT_WRCONFIG_PINMASK((uint16_t)PA05.bitmask);
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA04A_EIC_EXTINT4)|PORT_WRCONFIG_PINMASK((uint16_t)PA04.bitmask);
	EIC->EVCTRL.reg |=EIC_EVCTRL_EXTINTEO5|EIC_EVCTRL_EXTINTEO4;
	EIC->CONFIG[0].reg |= EIC_CONFIG_FILTEN5|EIC_CONFIG_SENSE5(0x3)|EIC_CONFIG_FILTEN4|EIC_CONFIG_SENSE4(0x3);
	EIC->INTENSET.reg|= EIC_INTENSET_EXTINT5| EIC_INTENSET_EXTINT4;
	EIC->CTRL.reg |=  EIC_CTRL_ENABLE;
	NVIC_SetPriority(SERCOM3_IRQn,0);
	NVIC_SetPriority(EIC_IRQn,1);
	NVIC_EnableIRQ(SERCOM3_IRQn);
	NVIC_EnableIRQ(EIC_IRQn);
	__enable_irq();
}
void findEdges(void){
	stepsSinceChange=0;
	currentDirection=0;
	prevDirection=0;
	prev2=0;
	isStalled=false;
	while(!isStalled){
		writePin(STEP,toggle);
		toggle = !toggle;
		delay_us(100);
		stepsSinceChange++;
	}
	delay_us(100);
	writePin(&PA25,false);
	stepsSinceChange=0;
	count=0;
	encTot=0;
	stepsTotal=0;
	currentDirection=0;
	prevDirection=0;
	prev2=0;
	direction=!direction;
	writePin(DIRPIN,direction);
	isStalled=false;
	while(!isStalled){
		writePin(STEP,toggle);
		toggle = !toggle;
		delay_us(100);
		stepsTotal++;
		stepsSinceChange++;
	}
	encTot=count;
	delay_us(100);
	writePin(&PA25,false);
	setup=false;
	
}
int main(void)
{
	changeClock();
	initPins();
	initRTC();
	EIC_setup();
	standalone_mode();
	findEdges();
	begin(identifier);
	
	steps1=(int)(stepsTotal*target);
	currentDirection=0;
	prevDirection=0;
	prev2=0;
	stepsSinceChange=0;
	direction=false;
	writePin(DIRPIN,direction);
	isStepping=true;
	isStalled=false;
	
	while(1){
		steps1=(int)((float)stepsTotal*requestedPosition());
		//counts=(int)((1-(float)count/encTot)*stepsTotal);
		/*
		if(isGrabbed){
			writePin(CFG6,true);
			delay_us(2000000);
			writePin(CFG6,false);
			isGrabbed=false;
			countdelta=0;
			isStalled=false;
			counts = (int)((1-(float)count/encTot)*stepsTotal);
		}
		*/
		if(counts==steps1){
			//writePin(CFG6,true);
			isStepping=false;
		}
		if(!isStalled){
			if(isStepping){
				if(counts<steps1){
					writePin(DIRPIN,false);
					stepsSinceChange=0;
					currentDirection=0;
					prevDirection=0;
					prev2=0;
					prev3=0;
					direction=false;
				}
				if(counts>steps1){
					writePin(DIRPIN,true);
					stepsSinceChange=0;
					currentDirection=0;
					prevDirection=0;
					prev2=0;
					prev3=0;
					direction=true;
				}
				stepsSinceChange++;
				writePin(STEP,toggle);
				toggle=!toggle;
				delay_us(60);
				if(direction){
					counts--;
				}
				if(!direction){
					counts++;
				}
			}
		}
		
	}
}