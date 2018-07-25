#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

// La dirección del MPU6050 puede ser 0x68 o 0x69, dependiendo
// del estado de AD0. Si no se especifica, 0x68 estará implicito
MPU6050 sensor;
#define out 13

// Valores RAW (sin procesar) del acelerometro y giroscopio en los ejes x,y,z
int16_t ax, ay, az;
int8_t ax1, ay1, az1;

//Valor de los offsets
int ax_o, ay_o, az_o;

//Variables usadas por el filtro pasa bajos
long f_ax, f_ay, f_az;
int p_ax, p_ay, p_az;
int counter = 0;

float m_ini = 0;
float m_fin = 0;

bool loop_s = false;

void setup() {
  Wire.begin();           //Iniciando I2C
  Serial.begin(2000000);    //Iniciando puerto serial
  sensor.initialize();    //Iniciando el sensor

  //Serial.println(sensor.testConnection() ? "Sensor iniciado correctamente" : "Error al iniciar el sensor");

  ax_o = sensor.getXAccelOffset();
  ay_o = sensor.getYAccelOffset();
  az_o = sensor.getZAccelOffset();
  //Serial.println("Envie cualquier caracter para empezar la calibracion");
  while (true) {
    if (Serial.available()) break;
  }
  Serial.println("Calibrando");
  calibrarSensor(1, 100);
  Serial.println("fin de la Calibracion");
  delay(100);
  Serial.println("Imprimiendo");
  pinMode(out, OUTPUT);
  digitalWrite(out, LOW);
}

void loop() {
  int seg = 0;
  while (loop_s == false)
  {
    seg = read_dataIn();
  }
  //sendStimValue(0, 1, STIM_ZERO + val_ma(data_int, CH1_MAX_POS));
  leerSensor(seg);

  loop_s = false;
}//Fin loop

/* Data reception */
int read_dataIn()
{
  int data_int = 0;
  if (Serial.available())
  {
    String data = Serial.readStringUntil('.');
    data_int = data.toInt();
    Serial.println("Data in: " + String(data));
    Serial.flush();
    loop_s = true;
  }
  return data_int;
}

//////////funcion calibrar
void calibrarSensor(int ms, int lecturas) {
  //Valor inicial de tiempo para calibracion
  int t_now = micros();
  int t_us = ms * 1000000;
  t_us = t_now + t_us;

  while (t_now < t_us) {
    sensor.getAcceleration(&ax, &ay, &az);

    // Filtrar las lecturas
    f_ax = f_ax - (f_ax >> 5) + ax;
    p_ax = f_ax >> 5;
    f_ay = f_ay - (f_ay >> 5) + ay;
    p_ay = f_ay >> 5;
    f_az = f_az - (f_az >> 5) + az;
    p_az = f_az >> 5;

    if (counter == lecturas) {
      //Calibrar el acelerometro a 1g en el eje z (ajustar el offset)
      if (p_ax > 0) {
        ax_o--;
      } else {
        ax_o++;
      }
      if (p_ay > 0) {
        ay_o--;
      } else {
        ay_o++;
      }
      if (p_az - 16384 > 0) {
        az_o--;
      } else {
        az_o++;
      }

      sensor.setXAccelOffset(ax_o);
      sensor.setYAccelOffset(ay_o);
      sensor.setZAccelOffset(az_o);

      counter = 0;
    }
    counter++;
    t_now = micros();
  }
}//fin calibrar sensor

void leerSensor(int ms) {
  //Valor inicial de tiempo para calibracion
  int t_now = micros();
  int t_us = ms * 1e6;
  t_us = t_now + t_us;

  digitalWrite(out, HIGH);
  int m_ini = micros ();

  while (t_now < t_us) {
    //sensor.getAcceleration(&ax, &ay, &az);
    //ax1 = sensor.getAccelerationX();
    //ay1 = sensor.getAccelerationY();
    az1 = sensor.getAccelerationZ();
    //String accel = String(ax1) + ":" + String(ay1) + ":" + String(az1);
    //Serial.println(accel);
    Serial.println(az1);
    t_now = micros();
  }

  digitalWrite(out, LOW);

}//fin leer sensor


