#include <atmel_start.h>
#define stepPin PA14
#define dirPin PA15
#define Enable PA16
void SPI_Send_Data(uint8_t data[5]){
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);
	spi_m_sync_enable(&SPI_0);
	io_write(io, data, 5);
}
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	gpio_set_pin_direction(stepPin, GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(dirPin, GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(Enable, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(dirPin,false);
	//gpio_set_pin_level(Enable,true);
	
	

	/* Replace with your application code */
	/*
	SPI send: 0xEC000100C3; // CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
	SPI send: 0x9000061F0A; // IHOLD_IRUN: IHOLD=10, IRUN=31 (max. current), IHOLDDELAY=6
	SPI send: 0x910000000A; // TPOWERDOWN=10: Delay before power down in stand still
	SPI send: 0x8000000004; // EN_PWM_MODE=1 enables stealthChop (with default PWM_CONF)
	SPI send: 0x93000001F4; // TPWM_THRS=500 yields a switching velocity about 35000 = ca. 30RPM
	SPI send: 0xF0000401C8; // PWM_CONF: AUTO=1, 2/1024 Fclk, Switch amplitude limit=200, Grad=1
	From Page 84 of Trinamic Driver Datasheet
	*/
	/*
	uint8_t data1[5] = { 0xEC, 0x00, 0x01, 0x00, 0xC3};
	uint8_t data2[5] = { 0x90, 0x00, 0x06, 0x1F, 0x0A};
	uint8_t data3[5] = { 0x91, 0x00, 0x00, 0x00, 0x0A};
	uint8_t data4[5] = { 0x80, 0x00, 0x00, 0x00, 0x04};
	uint8_t data5[5] = { 0x93, 0x00, 0x00, 0x01, 0xF4};
	uint8_t data6[5] = { 0xF0, 0x00, 0x04, 0x01, 0xC8};
	delay_ms(5);
	SPI_Send_Data(data1);
	delay_ms(5);
	SPI_Send_Data(data2);
	delay_ms(5);
	SPI_Send_Data(data3);
	delay_ms(5);
	SPI_Send_Data(data4);
	delay_ms(5);
	SPI_Send_Data(data5);
	delay_ms(5);
	SPI_Send_Data(data6);
	delay_ms(5);
	*/
	while (1) {
		gpio_set_pin_level(stepPin,true);
		delay_us(500);
		gpio_set_pin_level(stepPin,false);
		delay_us(500);
	}
}
