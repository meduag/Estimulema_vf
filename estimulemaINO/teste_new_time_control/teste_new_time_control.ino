#include <SPI.h>
/* Data Max and Min value DAC Channels */
#define CH1_MAX_POS 1432
#define CH1_MIN_NEG 1330
#define STIM_ZERO 2047
#define DAC_SS_PIN 10

//Variables to count time
elapsedMillis sinceTest1;
elapsedMillis sinceTest2;
elapsedMillis sinceTest3;

bool positive = 0, negative = 0, rest = 0;
int Am = 100;

void setup() {
  Serial.begin(2000000);
  bool loop_1 = true;
  Serial.println("Calibrando");
  while (loop_1) {
    if (sinceTest1 >= 5000) {
      Serial.println("Calibrado...");
      loop_1 = false;
      positive = 1;
      Serial.print("Start Positive");
      sendStimValue(0, 1, STIM_ZERO + val_ma(Am, CH1_MAX_POS));
      sinceTest1 = 0;
    }
  }
}

void loop() {
  if (positive) {
    if (sinceTest1 >= 1000) {
      positive = 0;
      negative = 1;
      Serial.println("Start Negative: ");
      sendStimValue(0, 1, STIM_ZERO - val_ma(Am, CH1_MIN_NEG));
      sinceTest2 = 0;
    }
  }
  if (negative) {
    if (sinceTest2 >= 1000) {
      negative = 0;
      rest = 1;
      Serial.println("Start Rest");
      sendStimValue(0, 1, STIM_ZERO + 20);
      sinceTest3 = 0;
    }
  }
  if (rest) {
    if (sinceTest3 >= 2000) {
      Serial.println("Start Positive: ");
      rest = 0;
      positive = 1;
      sendStimValue(0, 1, STIM_ZERO + val_ma(Am, CH1_MAX_POS));
      sinceTest1 = 0;
    }
  }
}

void sendStimValue(int address, int operation_mode, uint16_t value)
{
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

long val_ma(int Am, int limit)
{
  int y = 0;
  y = map(Am, 0, 100, 0, limit);
  return y;
}
