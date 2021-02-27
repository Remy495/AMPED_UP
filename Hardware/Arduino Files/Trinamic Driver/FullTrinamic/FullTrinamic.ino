#include <SPI.h>
#define DIR 2 //Goes to Step Pin on daughterboard(6)
#define STEP 3 //Goes to pin 7 on DB
#define MISO_CFG0 12 //pin 5 db ALSO MISO FOR SPI
#define MOSI_CFG1 11 //pin 4 db ALSO MOSI FOR SPI
#define SCK_CFG2 13 //pin 3 db ALSO SCK FOR SPI
#define SS_CFG3 10 //pin 2 db ALSO SS for SPI
#define CFG4 8 // pin 18 db
#define CFG5 9 //pin 19 db
#define CFG6 5 //Pin 22 DB Enable
#define SPI_MODE 6 //Pin 10 DB
//bool standalone = true;
uint8_t val[5]={0,0,0,0,0};
const uint8_t required[9][5] ={{0xEC,0x01,0x01,0x00,0xC2},{0x90,0x00,0x06,0x1F,0x0A},{0x91,0x00,0x00,0x00,0x0A},{0x80,0x00,0x00,0x00,0x03},{0x93,0x00,0x00,0x00,0x00}};
const uint8_t checkPins[5]={0x04,0x00,0x00,0x00,0x00};
void SPI_Send(uint8_t value[5]){
  digitalWrite(SS_CFG3, LOW); 
  for(int i =0; i<5; i++)
  {
    val[i] = SPI.transfer(value[i]); 
  }
  digitalWrite(SS_CFG3,HIGH);
  for(int i =0; i<5; i++){
    //Serial.print(val[i],HEX);
    //Serial.print(" ");
  }
  //Serial.println("");
}
void stepperSetup(){

  for(int i =0; i<9; i++){
      SPI_Send(required[i]);
      delay(100);
    }
  }
void standalone_mode()
{
  //standalone=true;
  digitalWrite(SPI_MODE,0);
  //set Chopper TOFF to 140 TCLK by tying MOSI_CFG1 to GND
  digitalWrite(MISO_CFG0,LOW);
  //Set to Standard Fullstep Operation by setting MOSI_CFG1 and SCK_CFG2 to GND
  digitalWrite(MOSI_CFG1,LOW);
  digitalWrite(SCK_CFG2,LOW);
  //set to use internal sense resistors and the analog input current on AIN
  digitalWrite(SS_CFG3,HIGH);
  //Chopper Hysteresis Setting(HEND=7 when CFG 4= GND)
  digitalWrite(CFG4,LOW);
  //set chopper blank time to TBL = 01 by setting CFG5->1
  digitalWrite(CFG5,HIGH);
  //Driver Enable
  digitalWrite(CFG6,LOW);
  digitalWrite(DIR,HIGH);
  digitalWrite(STEP,HIGH);
}
void spi_mode()
{
 //standalone=false;
 digitalWrite(SPI_MODE,1); 
 //Write CFG4 Low to disable dcstep
 digitalWrite(CFG4,LOW);
 //drive CFG 6 Low for Driver Enable(ONLY IN SPI MODE, THIS PIN WORKS AS A CONFIGURATION PIN IN STANDALONE)
 digitalWrite(CFG6,LOW);
 //Write SS High to ensure no signals get read by accident
 digitalWrite(SS_CFG3, HIGH); 
 SPI.setBitOrder(MSBFIRST);
 SPI.setDataMode(SPI_MODE3);
 SPI.begin();
  // Slow down the master a bit
 SPI.setClockDivider(SPI_CLOCK_DIV8);
 stepperSetup();
 digitalWrite(DIR,LOW);
 //digitalWrite(SPI_MODE,0);
}
void setup() 
{
 Serial.begin(9600);
 pinMode(DIR,OUTPUT);
 pinMode(STEP,OUTPUT);
 pinMode(SPI_MODE,OUTPUT);
 pinMode(MISO_CFG0,OUTPUT);
 pinMode(MOSI_CFG1,OUTPUT);
 pinMode(SCK_CFG2,OUTPUT);
 pinMode(SS_CFG3,OUTPUT);
 pinMode(CFG4,OUTPUT);
 pinMode(CFG5,OUTPUT);
 pinMode(CFG6,OUTPUT);
 //standalone_mode();
 spi_mode();

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(STEP,LOW);
  delayMicroseconds(10);
  //SPI_Send(checkPins);
  digitalWrite(STEP,HIGH);
  delayMicroseconds(10);

}
