#include <atmel_start.h>
#define stepPin PA14
#define dirPin PA15
#define IFA PA04
#define IFB PA05
#define IFC PA03
#define CFG0 PA08 //MISO
#define CFG1 PA11 //MOSI
#define CFG2 PA09 //SCK
#define CFG3 PA10 //SS
#define CFG4 PA18
#define CFG5 PA19
#define CFG6 PA16
#define SPI_MODE PA17
#define TEST PA06
uint8_t current;
uint8_t previous;
const uint8_t required[9][5] ={{0xEC,0x01,0x01,0x00,0xC2},{0x90,0x00,0x06,0x1F,0x0A},{0x91,0x00,0x00,0x00,0x0A},{0x80,0x00,0x00,0x00,0x03},{0x93,0x00,0x00,0x00,0x00}};//Desired Register Settings For Trinamic Driver
const int target = 0;
const int delay=2000;
const int threshold = 200;
long count = 0;
bool is_stepping=true;
int QEM[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
//This Code is in the process of being re-written for trinamic driver use rather than the MDR motor driver we used.

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	//gpio_set_pin_direction(CFG0,GPIO_DIRECTION_IN);
	//gpio_set_pin_direction(CFG1,GPIO_DIRECTION_OUT);
	//gpio_set_pin_direction(CFG2,GPIO_DIRECTION_OUT);
	//gpio_set_pin_direction(CFG3,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(CFG4,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(CFG5,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(CFG6,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(dirPin,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(stepPin,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(SPI_MODE,GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(IFA, GPIO_DIRECTION_IN);
	gpio_set_pin_direction(IFB, GPIO_DIRECTION_IN);
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);
	previous = gpio_get_pin_level(IFA)*2 + gpio_get_pin_level(IFB);//encoder stuff
	//standalone();
	//spi_setup();
	gpio_set_pin_direction(TEST, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(TEST,true);
	//main loop
	while(1){
		/*struct spi_xfer new;
		new.txbuf = "Hi\n";
		new.size=sizeof(new.txbuf);
		*/
		uint8_t data[6] = {0b01001000, 0b01100101, 0b01101100, 0b01101100, 0b01101111, 0b00100001};
		gpio_set_pin_level(CFG3,false);
		io_write(io,data,6);
		delay_ms(1);
		gpio_set_pin_level(CFG3,true);
		delay_ms(10000);
		
		//enc_read();
		//step();
	}
}
void SPI_Send_Data(uint8_t data[5]){
	
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);
	spi_m_sync_enable(&SPI_0);
	gpio_set_pin_level(CFG3,false);
	gpio_set_pin_level(CFG3,true);
}
void spi_setup(){//by driving SPI_MODE High, we enable SPI control to give it register configurations
	gpio_set_pin_level(SPI_MODE,true);
	gpio_set_pin_level(CFG4,false);
	gpio_set_pin_level(CFG6,false);
	gpio_set_pin_level(CFG3,true);//SS high for now
	gpio_set_pin_level(dirPin,false);
	for(int i=0; i<9;i++){
		SPI_Send_Data(required[i]);
		delay_ms(10);
	}
}
void standalone(){//By pulling SPI_MODE low, we dictate that the only thing the trinamic driver works is with the step/dir
	gpio_set_pin_level(SPI_MODE,false);
	gpio_set_pin_level(CFG0,false);
	gpio_set_pin_level(CFG1,false);
	gpio_set_pin_level(CFG2,false);
	gpio_set_pin_level(CFG3,true);
	gpio_set_pin_level(CFG4,false);
	gpio_set_pin_level(CFG5,true);
	gpio_set_pin_level(CFG6,false);
	gpio_set_pin_level(dirPin,true);
	gpio_set_pin_level(stepPin,false);
}
void dir_change(bool dir){
	gpio_set_pin_level(dirPin, dir);
}
void step(){
	if(is_stepping == true){
	gpio_set_pin_level(stepPin, true);
	delay_us(delay/2);
	gpio_set_pin_level(stepPin, false);
	delay_us(delay/2);
	}
}
	//indicates that our count value is within the threshold acceptance area, and we can stop moving until we recieve a new angle value from the teensy.
void stop_step(){
	is_stepping = false;
}
	//Performs the reading of the encoder, and decides which direction we should step in, as well as if we should stop stepping
void enc_read(){
	uint8_t bit1 = gpio_get_pin_level(IFA);
	uint8_t bit2 = gpio_get_pin_level(IFB);
	previous = current;
	current = bit1*2+bit2;
	count+=QEM[previous*4+current];
	if(count > target+threshold){dir_change(false);}
	else if(count < target-threshold){dir_change(true);}
	if(target-threshold < count){stop_step();}
	if(count < target + threshold){stop_step();}
	if(count >target+ threshold){is_stepping=true;}
	if(count<target-threshold){is_stepping=true;}
	if(count>2000){count--;}
	if(count<-2000){count++;}
}