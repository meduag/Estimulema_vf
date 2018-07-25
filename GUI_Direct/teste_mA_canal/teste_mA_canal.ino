#include <SPI.h>

#define CH1_MAX_POS 1883
#define CH1_MIN_NEG 1842
#define CH2_MAX_POS 2047
#define CH2_MIN_NEG 2047

#define DAC_SS_PIN 10
#define RELAY_CH_1 6
#define RELAY_CH_2 7

#define STIM_ZERO 2047
#define fin '>'

String data_in;
int mA_s = 0;
int pos_neg = 1;
int ch1_ch2 = 0;

void setup() {
  Serial.begin(2000000);
  Serial.setTimeout(0);

  SPI.begin();

  pinMode(DAC_SS_PIN, OUTPUT);
  pinMode(RELAY_CH_1, OUTPUT);
  pinMode(RELAY_CH_2, OUTPUT);

  digitalWrite(DAC_SS_PIN, 1);
  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);

  zeroChannels();
  delay(1000);
}

void loop() {
  read_dataIn();
  if (pos_neg) {
    
    if (ch1_ch2) {
      sendStimValue(1, 1, STIM_ZERO + val_ma(mA_s, CH2_MAX_POS));
    }
    
    if (ch1_ch2 == 0) {
      sendStimValue(0, 1, STIM_ZERO + val_ma(mA_s, CH1_MAX_POS));
    }
    
    if (ch1_ch2 == 2) {
      sendStimValue(1, 1, STIM_ZERO + val_ma(mA_s, CH2_MAX_POS));
      sendStimValue(0, 1, STIM_ZERO + val_ma(mA_s, CH1_MAX_POS));
    }
  } else {
    if (ch1_ch2) {
      sendStimValue(1, 1, STIM_ZERO - val_ma(mA_s, CH2_MIN_NEG));
    }
    
    if (ch1_ch2 == 0) {
      sendStimValue(0, 1, STIM_ZERO - val_ma(mA_s, CH1_MIN_NEG));
    }
    
    if (ch1_ch2 == 2) {
      sendStimValue(1, 1, STIM_ZERO - val_ma(mA_s, CH2_MIN_NEG));
      sendStimValue(0, 1, STIM_ZERO - val_ma(mA_s, CH1_MIN_NEG));
    }
  }
}

void read_dataIn() {
  if (Serial.available() != 0) {
    data_in = Serial.readStringUntil(fin);
    //int Am = data_in.toInt();
    //mA_s = map(Am, 0, 100, 0, 2047);
    mA_s = data_in.toInt();
    data_in = Serial.readStringUntil(fin);
    pos_neg = data_in.toInt();
    data_in = Serial.readStringUntil(fin);
    ch1_ch2 = data_in.toInt();
    Serial.readString();//limpia buffer
    Serial.flush();//espera hasta el ultimo byte de In Out
  }// fin if serial avaliable
}// fin function

void sendStimValue(int address, int operation_mode, uint16_t value) {
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

void zeroChannels() {
  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);
  sendStimValue(0, 1, (uint16_t) (STIM_ZERO + 10));
  sendStimValue(1, 1, (uint16_t) (STIM_ZERO + 10));
}

long val_ma(int Am, int limit) {
  int y = 0;
  y = map(Am, 0, 100, 0, limit);
  return y;
}
