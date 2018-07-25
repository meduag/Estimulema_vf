/*

AUTHOR: Hazim Bitar (techbitar)
DATE: Aug 29, 2013
LICENSE: Public domain (use at your own risk)
CONTACT: techbitar at gmail dot com (techbitar.com)

*/


//#include <SoftwareSerial.h>

//SoftwareSerial BTSerial(10, 11); // RX | TX

void setup() 
{
  pinMode(3, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(3, HIGH); 
  Serial.begin(9600);
  Serial.println("Enter AT commands:");
  Serial1.begin(115200);  // HC-05 default speed in AT command more
}

void loop()
{

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (Serial1.available())
    Serial.write(Serial1.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available()){
    Serial1.write(Serial.read());
    //Serial.write(Serial.read());
  }
}


