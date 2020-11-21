
#include <Wire.h>
int data;
void setup() {
  // put your setup code here, to run once:
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}
void receiveEvent(int howMany)
{
    data = Wire.read(); // receive byte as a character         // print the character
    Serial.println(data);
  
}
