/*
 * TempSPI.h
 *
 * Created: 3/25/2021 5:13:47 PM
 *  Author: hanle
 */ 


#ifndef TEMPSPI_H_
#define TEMPSPI_H_
#include "Pins_Setup.h"
#include "Timing.h"
#define SPI_CLK 8000000
#define SPI_BAUD 1000000
void spi_clock_init(){
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
}
void spi_master_init(){
	uint16_t baud = ((float)SPI_CLK / (float)(2 * SPI_BAUD)) - 1;
	spi_clock_init();
	//Pin Configurations(Pin PA08 Is Input, and the other 2 are outputs for SPI, we are controlling our SS through software, so it is normally configred instead of PMUX)
	setPin(&PA10,OUTPUT,NORMAL,PULL_DOWN);
	writePin(&PA10,true);
	PORT->Group[0].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA08C_SERCOM0_PAD0)|PORT_WRCONFIG_INEN|PORT_WRCONFIG_PINMASK((uint16_t)(PA08.bitmask));
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA09C_SERCOM0_PAD1)|PORT_WRCONFIG_PINMASK((uint16_t)PA09.bitmask);
	PORT->Group[0].WRCONFIG.reg =  PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PMUX(MUX_PA11C_SERCOM0_PAD3)|PORT_WRCONFIG_PINMASK((uint16_t)PA11.bitmask);
	
	//Reset the CTRLA Register to make CTRLA.enable=0 and allow for sercom configs
	SERCOM0->SPI.CTRLA.reg = 0;
	SERCOM0->SPI.CTRLA.reg =  SERCOM_SPI_CTRLA_MODE_SPI_MASTER|SERCOM_SPI_CTRLA_CPOL|SERCOM_SPI_CTRLA_CPHA|SERCOM_SPI_CTRLA_DIPO(0)|SERCOM_SPI_CTRLA_DOPO(2);
	while(SERCOM0->SPI.SYNCBUSY.bit.CTRLB);
	SERCOM0->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
	while(SERCOM0->SPI.SYNCBUSY.bit.CTRLB);
	//Eabling Interrupt bits for later use
	SERCOM0->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_ERROR| SERCOM_SPI_INTENSET_RXC|SERCOM_SPI_INTENSET_TXC | SERCOM_SPI_INTENSET_DRE;
	SERCOM0->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD(baud);
	//After all register settings are set, we set the enable bit, which restricts their changes later
	SERCOM0->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
	while(SERCOM0->SPI.SYNCBUSY.bit.ENABLE);
}
void spi_send_config(const uint8_t data[5], volatile uint8_t response[5]){
	delay_us(10);
	writePin(&PA10,false);
	delay_us(10);
	for(int i = 0; i<5; i++){
		while(!(SERCOM0->SPI.INTFLAG.bit.DRE));//Wait For data register to be empty
		SERCOM0->SPI.DATA.reg = SERCOM_SPI_DATA_DATA(data[i]);
		
		while(!(SERCOM0->SPI.INTFLAG.bit.TXC));//Wait for transfer complete interrupt
		SERCOM0->SPI.INTFLAG.reg |= SERCOM_SPI_INTFLAG_TXC;
		
		while(!(SERCOM0->SPI.INTFLAG.bit.RXC));//Wait for receive complete interrupt
		SERCOM0->SPI.INTFLAG.reg |= SERCOM_SPI_INTFLAG_RXC;
		response[i] = (uint8_t)SERCOM0->SPI.DATA.reg;
		delay_us(50);
	}
	delay_us(10);
	writePin(&PA10,true);
	delay_us(30);
}


#endif /* TEMPSPI_H_ */