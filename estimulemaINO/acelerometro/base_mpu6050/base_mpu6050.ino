// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int ms;
bool loop_s = false;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);
}

void loop() {
  int seg = 0;
  Serial.println("Digite el valor en seg");
  while (loop_s == false) {
    seg = read_dataIn();
  }

  //Variaveis para armazenar valores dos sensores
  int t_now = micros();
  int t_us = seg * 1e6;
  t_us = t_now + t_us;

  if(t_now < t_us) {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
    //Serial.println("Entro en el while");
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    //Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    //GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    //GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    //GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    //Serial.print("AcX = ");
    String acel = String (AcX) + ";" + String (AcY) + ";" + String (AcZ);
    //Serial.print(AcX);
    //Serial.print(" | AcY = ");
    //Serial.print(";");
    //Serial.print(AcY);
    //Serial.print(" | AcZ = ");
    //Serial.print(";");
    //Serial.println(AcZ);
    Serial.println(acel);
    //Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
    //Serial.print(" | GyX = "); Serial.print(GyX);
    //Serial.print(" | GyY = "); Serial.print(GyY);
    //Serial.print(" | GyZ = "); Serial.println(GyZ);
    //delay(1);
    t_now = micros();
  }
  loop_s = false;
}

/* Data reception */
int read_dataIn() {
  int data_int = 0;
  if (Serial.available()) {
    String data = Serial.readStringUntil('.');
    data_int = data.toInt();
    Serial.println("Data in: " + String(data));
    //Serial.flush();
    loop_s = true;
  }
  return data_int;
}
