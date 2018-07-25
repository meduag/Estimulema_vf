/*#define Pin_Sync_Data 8
long cont = 0;
void setup() {
  Serial.begin(2000000);
  pinMode(Pin_Sync_Data, OUTPUT);
  digitalWrite(Pin_Sync_Data, 0);
}

void loop() {
  Serial.print("Contador: ");
  Serial.println(cont);
  cont += 1;
  digitalWrite(Pin_Sync_Data, 1);
  delay(5000);
  digitalWrite(Pin_Sync_Data, 0);
  delay(5000);
}*/

#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

//#define Pin_Inter 23
//#define Pin_Led_Control 13

//MPU6050 accelgyro;
MPU6050 accel;

int16_t ax, ay, az;
bool sair = true, control = true;
//bool capture = false, loops = true;
//long t_us;
//int c = 0;

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
  Serial.begin(2000000);
  Serial.println("Initializing I2C devices...");
  //accelgyro.initialize();
  accel.initialize();
  Serial.println("Testing device connections...");
  //Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  Serial.println(accel.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  //pinMode(Pin_Inter, INPUT);
  //pinMode(Pin_Led_Control, OUTPUT);
  //attachInterrupt(Pin_Inter, Communication_data, CHANGE);
  //digitalWrite(Pin_Led_Control,1);


}

void loop() {
	loops_inicio();

	long tus = 1000000;
	if(control){
		Serial.print("Inicio micros: ");
		Serial.println(micros());
		tus = micros() + tus;
  		while(micros() < tus && control == true) {
	  	    accel.getAcceleration(&ax, &ay, &az);
	      	Serial.print(ax);
	      	Serial.print(":");
	      	Serial.print(ay);
	      	Serial.print(":");
	      	Serial.println(az);
		  	}
  	  	Serial.print("Fin micros: ");
		Serial.println(micros());
	}
  	control = false;
}

void loops_inicio(){
	if(sair){
		Serial.println("Insira una tecla para conticuar...");
	}
	while(sair){
		
		if (Serial.available() != 0) {
			sair = false;
		}
	}
}



