#include <Wire.h>

int16_t ax, ay, az;
bool c = true;
unsigned long tf = 0;

void setup() {
  Serial.begin(2000000);
  Wire.setClock(400000);
  Wire.begin();
  delay(250);

  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
}

void loop() {
  tf = micros() + 1e6;
  if (c) {
    Serial.print("Micros: ");
    Serial.print(micros());
    Serial.print(" Tf: ");
    Serial.println(tf);
    delay(1000);
  }
  
  tf = micros() + 15e6;
  while (micros() < tf && c) {
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission();
    Wire.requestFrom(0x68, 6);
    ax = Wire.read() << 8 | Wire.read();
    ay = Wire.read() << 8 | Wire.read();
    az = Wire.read() << 8 | Wire.read();

    Serial.print(ax);
    Serial.print(";");
    Serial.print(ay);
    Serial.print(";");
    Serial.println(az);
    //delay(1);
  }
  c = false;

}
