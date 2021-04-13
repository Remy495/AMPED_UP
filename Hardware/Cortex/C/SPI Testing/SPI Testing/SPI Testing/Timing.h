/*
 * Timing.h
 *
 * Created: 3/23/2021 10:44:36 AM
 *  Author: hanle
 */ 
//This File Will include needed declarations and functions to use the RTC to make a delay us function
#ifndef TIMING_H_
#define TIMING_H_
void changeClock(){
	/* Set the correct number of wait states for 48 MHz @ 3.3v */
	NVMCTRL->CTRLB.bit.RWS = 1;
	/* This works around a quirk in the hardware (errata 1.2.1) -
	   the DFLLCTRL register must be manually reset to this value before
	   configuration. */
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

	/* Write the coarse and fine calibration from NVM. */
	uint32_t coarse =
		((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
	uint32_t fine =
		((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;

	SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);

	/* Wait for the write to finish. */
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};
	/* Enable the DFLL */
	SYSCTRL->DFLLCTRL.bit.ENABLE = 1;

	/* Wait for the write to finish */
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};
		
	/* Setup GCLK0 using the DFLL @ 48 MHz */
	GCLK->GENCTRL.reg =
		GCLK_GENCTRL_ID(0) |
		GCLK_GENCTRL_SRC_DFLL48M |
		/* Improve the duty cycle. */
		GCLK_GENCTRL_IDC |
		GCLK_GENCTRL_GENEN;

	/* Wait for the write to complete */
	while(GCLK->STATUS.bit.SYNCBUSY);
}
void initRTC(){
	//A generic clock (GCLK_RTC) is required to clock the RTC. This clock must be configured and enabled in the Generic Clock Controller before using the RTC(Gen Clock ID 4, and )
	
	GCLK->GENDIV.reg  = GCLK_GENDIV_ID(0)|GCLK_GENDIV_DIV(6);
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_RTC | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
	
	/*GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(4)|  GCLK_GENCTRL_GENEN;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_RTC |GCLK_CLKCTRL_GEN_GCLK4|GCLK_CLKCTRL_CLKEN; */
	PM->APBAMASK.reg |= PM_APBAMASK_RTC;
	//Setting Settings for RTC*/

	RTC->MODE0.CTRL.reg = 0x0;
	//RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MATCHCLR;
	RTC->MODE0.INTENSET.reg |= RTC_MODE0_INTENSET_CMP(48);
	//RTC->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_CMPEO(48);
	RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE;
	
}
void delay_us(int num){
	for(int i = num; i>0; i--){
		RTC->MODE0.COMP->reg=0;
		RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP(48);
		while(!RTC->MODE0.INTFLAG.reg && RTC_MODE0_INTFLAG_CMP(48));
	}
	
}


#endif /* TIMING_H_ */