/*
 * IncFile1.h
 *
 * Created: 3/11/2021 6:34:03 PM
 *  Author: hanle
 */ 


#ifndef SERCOMCONFIG_H_
#define SERCOMCONFIG_H_
#include "Pins_Setup.h"
enum SercomIndex{SERCOM_0,SERCOM_1,SERCOM_2,SERCOM_3};//4 Sercom Modules that can be Configured
enum SercomClkId{CLKS0 = GCLK_CLKCTRL_ID_SERCOM0_CORE, CLKS1 = GCLK_CLKCTRL_ID_SERCOM1_CORE, CLKS2 = GCLK_CLKCTRL_ID_SERCOM2_CORE, CLKS3= GCLK_CLKCTRL_ID_SERCOM3_CORE};;
uint8_t SERCOM_PIN_COUNT=4;
struct SercomConfig{//Sercom Data Config Struct for Bitmasks, Clock IDs and Pin identifiers
	Sercom* regPtr;
	enum SercomIndex _SercomIndex;
	enum SercomClkId _SercomClkId;
	uint8_t GenericClockID;
	uint32_t POWER_MANAGER_BITMASK;
	uint32_t CLOCK_CONTROL_ID;
	struct pin defaultPins[4];
	struct pin altPins[4] ;
};
void SercomSetIndex(SercomConfig* config,enum SercomIndex sercomIndex){
	if(sercomIndex==SERCOM_0){
		config->regPtr=SERCOM0;
		config->_SercomClkId = GCLK_CLKCTRL_ID_SERCOM0_CORE;
		config->defaultPins={PA08,PA09,PA10,PA11};
		config->altPins={PA04,PA05,PA06,PA07};
	}
	if(sercomIndex==SERCOM_1){
		config->regPtr=SERCOM1;
		config->_SercomClkId = GCLK_CLKCTRL_ID_SERCOM1_CORE;
	}
	if(sercomIndex==SERCOM_2){
		config->regPtr=SERCOM2;
		config->_SercomClkId = GCLK_CLKCTRL_ID_SERCOM2_CORE;
	}
	if(sercomIndex==SERCOM_3){
		config->regPtr=SERCOM3;
		config->_SercomClkId = GCLK_CLKCTRL_ID_SERCOM3_CORE;
	}
}
void SercomEnable(SercomConfig* sercomConfig){//Enables Clock in PM and in GCLK
	PM->APBCMASK.reg |= sercomConfig->POWER_MANAGER_BITMASK;
	GCLK->CLKCTRL.reg = sercomConfig->_SercomClkId | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(sercomConfig->GenericClockID);
}
void SercomSpiMSetup(SercomConfig* sercomConfig){
	sercomConfig->regPtr->SPI.CTRLA.reg =0;// If Enable is 1 by any chance, clear it, so we can access registers. 
	sercomConfig->regPtr->SPI.CTRLA.reg |=  SERCOM_SPI_CTRLA_MODE_SPI_MASTER|SERCOM_SPI_CTRLA_CPOL|SERCOM_SPI_CTRLA_CPHA|SERCOM_SPI_CTRLA_DOPO(2)|SERCOM_SPI_CTRLA_IBON |SERCOM_SPI_CTRLA_RUNSTDBY;
	sercomConfig->regPtr->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN | SERCOM_SPI_CTRLB_SSDE;
	sercomConfig->regPtr->SPI.BAUD = 809;
	sercomConfig->regPtr->SPI.INTENSET = SERCOM_SPI_INTENSET_ERROR|SERCOM_SPI_INTENSET_SSL|SERCOM_SPI_INTENSET_RXC|SERCOM_SPI_INTENSET_TXC|SERCOM_SPI_INTENSET_DRE;
	setPin(PA08,SERCOM_E);
	setPin(PA09, SERCOM_O);
	setPin(PA10,SERCOM_E);
	setPin(PA11,SERCOM_O);
	//Once we are set up, enable SPI Master
	sercomConfig->regPtr->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
}
void SpiSend(SercomConfig* config, uint8_t data){
	
	config->regPtr->SPI->DATA = data;
	while(!(config->regPtr->SPI->INTFLAG.reg && SERCOM_SPI_INTFLAG_DRE));
}
uint8_t SpiReceive(){
	return 0;
}
#endif 