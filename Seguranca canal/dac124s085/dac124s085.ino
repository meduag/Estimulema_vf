#include <SPI.h>

//configuracion datos del dac

#define DAC_SS_PIN 10
//#define PinInt 23
#define t 2000000
#define t2 1000
int teste = 0;

//int const x = 100;
//int y = 0, of = 100, off = 0;
//boolean c = true;
//int t = 50000;

void setup() {
  pinMode(DAC_SS_PIN, OUTPUT);
  //pinMode(PinInt, INPUT);
  digitalWrite(DAC_SS_PIN, HIGH);
  //attachInterrupt(PinInt, interrupcion, RISING);
  //SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  //y = map(x, 0, 100, 0, 2047);
  //off = map(of, 0, 100, 0, 2047);
  //Serial.print("Valor de Y: ");
  //Serial.println(y);
}

void loop() {

  //while (c == 100) {
  sendStimValue(0, 1, teste);
  sendStimValue(1, 1, teste);
  sendStimValue(2, 1, 4095);
  sendStimValue(3, 1, 4095);
  
 /* sendStimValue(0, 1, 4095);
  sendStimValue(1, 1, 4095);
  sendStimValue(2, 1, 4095);
  sendStimValue(3, 1, 4095);
  //delayMicroseconds(t);
  delay(t2);
  sendStimValue(0, 1, 2047);
  sendStimValue(1, 1, 2047);
  sendStimValue(2, 1, 2047);
  sendStimValue(3, 1, 2047);
  delay(t2);
  //delayMicroseconds(t);
  sendStimValue(0, 1, 0);
  sendStimValue(1, 1, 0 );
  sendStimValue(2, 1, 0);
  sendStimValue(3, 1, 0);
  delay(t2);
  //delayMicroseconds(t);
  sendStimValue(0, 1, 2047);
  sendStimValue(1, 1, 2047);
  sendStimValue(2, 1, 2047);
  sendStimValue(3, 1, 2047);
  //4delay(t);
  delayMicroseconds(t);


  //delayMicroseconds(199000);
  //c = c + 1;
  //}
*/
}

/*void interrupcion() {
  sendStimValue(0, 1, 2047);
  c = !c;
  while (c == false) {
    Serial.println("Parou");
    delay(1000);
  }

  }*/

void sendStimValue(int address, int operation_mode, uint16_t value)
{
  byte valueToWriteH = 0;
  byte valueToWriteL = 0;

  valueToWriteH = highByte(value);
  valueToWriteH = 0b00001111 & valueToWriteH;
  valueToWriteH = (address << 6 ) | (operation_mode << 4) | valueToWriteH;
  valueToWriteL = lowByte(value);

  digitalWrite(DAC_SS_PIN, LOW);
  SPI.transfer(valueToWriteH);
  SPI.transfer(valueToWriteL);
  digitalWrite(DAC_SS_PIN, HIGH);
}
