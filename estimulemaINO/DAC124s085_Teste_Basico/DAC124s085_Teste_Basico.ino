#include <SPI.h>
#define DAC_SS_PIN 10
#define CH1_MAX_POS 1432
#define CH1_MIN_NEG 1330
#define STIM_ZERO 2047
#define RELAY_CH_1 6
#define RELAY_CH_2 7
#define Pin_Inter 23
// Parametros de estimulação, teste de resistencia
int mA = 1;
long pw = 500;
long pw_r = 19000;
bool loop_control = false;
// teste basico 1
bool cr = 1, s_c = 1;
int lim_pw = 1000, pwrc = 1000;
int lim_ma = 5;
// long t_stop = 0;
void setup()
{
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

void loop()
{
  // basico 1
  /* while(loop_control){
  digitalWrite(RELAY_CH_1, 1);
  sendStimValue(0, 1, STIM_ZERO + val_ma(mA, CH1_MAX_POS));
  delayMicroseconds(pw);
  sendStimValue(0, 1, STIM_ZERO - val_ma(mA, CH1_MIN_NEG));
  delayMicroseconds(pw);
  sendStimValue(0, 1, STIM_ZERO);
  delayMicroseconds(pw_r);
  }
  if(loop_control == false){
  zeroChannels();
  }*/
  // basico 2
  if (cr && s_c)
  {
    // Serial.println("Start Chronaxie");
    // Serial.println("Inicia captura dados Cronaxia");
    // digitalWrite(Pin_Sync_Data, 1); // Inicia a captura
    unsigned int pw_c = 0; // count the variable value of pw
    lim_pw = lim_pw + 1;
    Serial.print("s0;");
    Serial.print(pw_c);
    Serial.print(";");
    Serial.println(micros());
    delay(5000);
    pw_c = 100;
    while (pw_c < lim_pw && s_c)
    {
      Serial.print("s0;");
      Serial.print(pw_c);
      Serial.println(";0");
      for(int i=0; i< 1000; i++){
        rheobase_chronaxie(lim_ma, pw_c, pwrc);
      }
      digitalWrite(RELAY_CH_1, 0);
      delay(5000);
      pw_c += 100;
      if (pw_c >= lim_pw && s_c)
      {
        // digitalWrite(Pin_Sync_Data, 0);
        Serial.print("s0;");
        Serial.print(pw_c - 100);
        Serial.print(";");
        Serial.println(micros());
        cr = 0;
      }
    }
    // Fin while limit pw
  }
  // end if cr
}

void rheobase_chronaxie(int mA_s, int t1, int t2)
{
  unsigned long t_stop = 0;
  digitalWrite(RELAY_CH_1, 1);
  // digitalWrite(Pin_signal_Control, 1);
  sendStimValue(0, 1, STIM_ZERO + val_ma(mA_s, CH1_MAX_POS));
  //Serial.print("s1;");
  //Serial.print(mA_s);
  //Serial.print(";");
  //Serial.println(micros());
  t_stop = micros() + t1;
  while (micros() <= t_stop && s_c)
  {
    //Serial.print("Micros ini: ");
    //Serial.println(micros());
    read_dataIn();
    //Serial.print("Micros fin: ");
    //Serial.println(micros());
  }
  //Serial.print("s1;");
  //Serial.print(mA_s);
  //Serial.print(";");
  //Serial.println(micros());
  sendStimValue(0, 1, STIM_ZERO - val_ma(mA_s, CH1_MIN_NEG));
  //Serial.print("s2;");
  //Serial.print(mA_s);
  //Serial.print(";");
  //Serial.println(micros());
  t_stop = micros() + t1;
  while (micros() <= t_stop && s_c)
  {
    read_dataIn();
  }
  //Serial.print("s2;");
  //Serial.print(mA_s);
  //Serial.print(";");
  //Serial.println(micros());
  //digitalWrite(RELAY_CH_1, 0);
  sendStimValue(0, 1, STIM_ZERO + 5);
  //Serial.print("s0;");
  //Serial.print(mA_s);
  //Serial.println(";0");
  // digitalWrite(Pin_signal_Control, 0);
  t_stop = micros() + t2;
  while (micros() <= t_stop && s_c)
  {
    read_dataIn();
  }
}

void read_dataIn()
{
  if (Serial.available() != 0)
  {
    Serial.println("Hola, llego un dato");
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

void Stop_functions()
{
  Serial.println("Interrupcion activada");
  loop_control = !loop_control;
  delay(200);
}

long val_ma(int Am, int limit)
{
  int y = 0;
  y = map(Am, 0, 100, 0, limit);
  return y;
}

void zeroChannels()
{
  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);
  sendStimValue(0, 1,(uint16_t) (STIM_ZERO + 20));
  sendStimValue(1, 1,(uint16_t) (STIM_ZERO + 20));
}
