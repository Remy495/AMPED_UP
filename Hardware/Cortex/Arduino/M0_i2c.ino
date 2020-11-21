#include <Wire.h>
#define stepPin 31
#define dirPin 32
int data=2;
void setup()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  digitalWrite(dirPin,HIGH);
}

void loop()
{
  //Serial.println("In Loop");
  digitalWrite(stepPin,HIGH);
  delay_us(50);
  digitalWrite(stepPin,LOW);
  delay_us(50);
}
void delay_us(int length) {
      long start = micros();
      while(length > micros() - start) {
        if (micros() < start) { break; }
      }
}
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
    data = Wire.read(); // receive byte as a character         // print the character
    if(data == 1){
      digitalWrite(dirPin,HIGH);
    }
    if(data == 0){
      digitalWrite(dirPin,LOW);
    }
  
}
