#include <SPI.h>

#define DAC_SS_PIN 10

#define CH1_MAX_POS 1432
#define CH1_MIN_NEG 1330
#define STIM_ZERO 2047

#define RELAY_CH_1 6
#define RELAY_CH_2 7

#define Pin_Inter 23 

int mA = 10;
long pw = 500;
long pw_r = 19000;
bool loop_control = false;

void setup(){
	Serial.begin(2000000);
	pinMode(DAC_SS_PIN, OUTPUT);
	digitalWrite(DAC_SS_PIN, 1);

	pinMode(RELAY_CH_1, OUTPUT);
  	pinMode(RELAY_CH_2, OUTPUT);

    digitalWrite(RELAY_CH_1, 0);
  	digitalWrite(RELAY_CH_2, 0);

  	attachInterrupt(Pin_Inter, Stop_functions, RISING);

	SPI.begin();
	zeroChannels();
}

void loop(){
	while(loop_control){
		sendStimValue(0, 1, STIM_ZERO + val_ma(mA, CH1_MAX_POS));
		delayMicroseconds(pw);
		sendStimValue(0, 1, STIM_ZERO - val_ma(mA, CH1_MIN_NEG));
		delayMicroseconds(pw);
		sendStimValue(0, 1, STIM_ZERO + 10);
		delayMicroseconds(pw_r);
	}
	if(loop_control == false){
		zeroChannels();
	}
}

void sendStimValue(int address, int operation_mode, uint16_t value){
	byte valueToWriteH = 0;
	byte valueToWriteL = 0;
	valueToWriteH = highByte(value);
	valueToWriteH = 0b00001111 & valueToWriteH;
	valueToWriteH = (address << 6) | (operation_mode << 4) | valueToWriteH;
	valueToWriteL = lowByte(value);
	digitalWrite(DAC_SS_PIN, LOW);
	SPI.transfer(valueToWriteH);
	SPI.transfer(valueToWriteL);
	digitalWrite(DAC_SS_PIN, HIGH);
}

void Stop_functions() {
  Serial.println("Interrupcion activada");
  loop_control = !loop_control;
}

long val_ma(int Am, int limit) {
  int y = 0;
  y = map(Am, 0, 100, 0, limit);
  return y;
}

void zeroChannels(){
	digitalWrite(RELAY_CH_1, 0);
  	digitalWrite(RELAY_CH_2, 0);
	sendStimValue(0, 1,(uint16_t) (STIM_ZERO + 20));
	sendStimValue(1, 1,(uint16_t) (STIM_ZERO + 20));
}
