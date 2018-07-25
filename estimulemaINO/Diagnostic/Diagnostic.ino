#include <SPI.h>

#define DAC_SS_PIN 10
#define RELAY_CH_1 6
#define RELAY_CH_2 7
#define Pin_Sync_Data 8
#define Pin_signal_Control 14
#define Pin_Inter 23 
#define Gpio5 8

#define CH1_MAX_POS 1432
#define CH1_MIN_NEG 1330
#define STIM_ZERO 2047
#define fin '>'

#define pw 5e5
#define pwrc 2e6
//#define limit_pw 500 // Modificar aqui para colocar el limite de Cronaxia
//#define lim_mA 5 // Modificar aqui para colocar el limite de Reobase
//#define mA_r 5 // mA fijo para Cronaxia

String data;
bool loop_s = true;
long t_stop;
bool control_rh = true;
bool control_cr = true;

//Para selecionar y controlar el flujo do programa
String rh_cr = "0";
String loop_close = "0";

String lim_mA_Data_In = "0";
String lim_pw_c_Data_In = "0";
int lim_mA_In = 5; // Modificar aqui para colocar el limite de Reobase
int lim_pw_c_In = 5; // Modificar aqui para colocar el limite de Cronaxie

void setup() {
  Serial.begin(2000000);
  Serial.setTimeout(0);
  
  SPI.begin();

  pinMode(DAC_SS_PIN, OUTPUT);
  pinMode(Pin_Sync_Data, OUTPUT);
  pinMode(Pin_signal_Control, OUTPUT);
  pinMode(RELAY_CH_1, OUTPUT);
  pinMode(RELAY_CH_2, OUTPUT);

  pinMode(Pin_Inter, INPUT);


  digitalWrite(DAC_SS_PIN, 1);

  digitalWrite(Pin_Sync_Data, 0); // Configurar para hacer el rising en el pon de int
  digitalWrite(Pin_signal_Control, 0); // Configurar para hacer el rising en el pon de int


  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);

  attachInterrupt(Pin_Inter, Stop_functions, RISING);

  zeroChannels();
}

void loop() {
  while (loop_s) {
    read_dataIn();
  }

  int ma = 0;
  int pw_c = 0;
  long ini = 0;

  while(rh_cr == "R" && control_rh == true){
    //Serial.println("Start Rheobase");
    //Serial.println("Inicia captura dados Reobase");
    digitalWrite(Pin_Sync_Data, 1);

    Serial.print("M");
    Serial.print(ma);
    Serial.print(";T");
    Serial.print(micros());
    Serial.println(";");
    delay(1000);
    ma = 1;
    while (ma < lim_mA_In){
      Serial.print("M");
      Serial.print(ma);
      Serial.println(";T0;");
      rheobase_chronaxie(ma, pw, pwrc);
      ma +=1;
      if(ma >= lim_mA_In){
        digitalWrite(Pin_Sync_Data, 0);
        Serial.print("M");
        Serial.print(ma);
        Serial.print(";T");
        Serial.print(micros());
        Serial.println(";");
        control_rh = false;
        rh_cr = "M";
      }
    } // Fin while
  }
  
  while(rh_cr == "C" && control_cr == true){
    //Serial.println("Start Chronaxie");
    //Serial.println("Inicia captura dados Cronaxia");
    digitalWrite(Pin_Sync_Data, 1); //Inicia a captura

    Serial.print("M");
    Serial.print(pw_c);
    Serial.print(";T");
    Serial.print(micros());
    Serial.println(";");
    delay(1000);
    pw_c = 100;
    while(pw_c < lim_pw_c_In) {
      Serial.print("M");
      Serial.print(pw_c);
      Serial.println(";T0;");
      rheobase_chronaxie((lim_mA_In * 2), pw_c, pwrc);
      pw_c += 100;
      if(pw_c >= lim_pw_c_In){
        digitalWrite(Pin_Sync_Data, 0);
        Serial.print("M");
        Serial.print(pw_c);
        Serial.print(";T");
        Serial.print(micros());
        Serial.println(";");
        control_rh = false;
        rh_cr = "M";
      }
    } // Fin while
  }
  //Habilita el ciclo de lectura nuevamente
  loop_s = true;
}

void read_dataIn() {
  if (Serial.available() != 0) {
    rh_cr = Serial.readStringUntil(fin);
    //Serial.print("Data in rh_cr: ");
    //Serial.println(rh_cr);
    loop_close = Serial.readStringUntil(fin);
    //Serial.print("Data in loop_close: ");
    //Serial.println(loop_close);
    lim_mA_Data_In = Serial.readStringUntil(fin);
    lim_mA_In = lim_mA_Data_In.toInt() + 1;
    //Serial.println(lim_mA_In);
    lim_pw_c_Data_In = Serial.readStringUntil(fin);
    lim_pw_c_In = lim_pw_c_Data_In.toInt() + 1;
    //Serial.println(lim_pw_c_In);

    Serial.readString();//limpia buffer 
    Serial.flush();//espera hasta el ultimo byte de In Out 

    if(loop_close == "Y"){
      digitalWrite(Pin_Sync_Data, 0); // para la captura
      //Serial.println("Parar captura de dados");
      rh_cr = "M";
      loop_close = "M";
      control_cr = false;
      control_rh = false;
    }else{
      control_cr = true;
      control_rh = true;
    }
    //Sale del ciclo de captura de datos
    loop_s = false;
  }
}


void rheobase_chronaxie(int mA_ch1, int t1, int t2) {
  digitalWrite(RELAY_CH_1, 1);
  digitalWrite(Pin_signal_Control, 1);
  sendStimValue(0, 1, STIM_ZERO + val_ma(mA_ch1, CH1_MAX_POS));
  Serial.print("P");
  Serial.print(";T");
  Serial.print(micros());
  Serial.println(";");
  t_stop = micros() + t1;
  while (micros() <= t_stop && loop_close == "N") {
    read_dataIn();
  }
  Serial.print("P");
  Serial.print(";T");
  Serial.print(micros());
  Serial.println(";");
  sendStimValue(0, 1, STIM_ZERO - val_ma(mA_ch1, CH1_MIN_NEG));
  Serial.print("N");
  Serial.print(";T");
  Serial.print(micros());
  Serial.println(";");
  t_stop = micros() + t1;
  while (micros() <= t_stop && loop_close == "N") {
    read_dataIn();
  }
  Serial.print("N");
  Serial.print(";T");
  Serial.print(micros());
  Serial.println(";");
  digitalWrite(RELAY_CH_1, 0);
  sendStimValue(0, 1, STIM_ZERO);
  Serial.println("R");
  digitalWrite(Pin_signal_Control, 0);
  t_stop = micros() + t2;
  while (micros() <= t_stop && loop_close == "N") {
    read_dataIn();
  }
}

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

long val_ma(int Am, int limit) {
  int y = 0;
  y = map(Am, 0, 100, 0, limit);
  return y;
}

void Stop_functions() {
  zeroChannels();
  Serial.println("Interrupcion activada");
  Serial.println("Para captura de datos por interrupcion");
  digitalWrite(Pin_Sync_Data, 0);
  digitalWrite(Pin_signal_Control, 0);
  loop_close = "Y";
  rh_cr = "M";
}

void zeroChannels() {
  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);
  sendStimValue(0, 1, (uint16_t) (STIM_ZERO + 10));
  sendStimValue(1, 1, (uint16_t) (STIM_ZERO + 10));
}

/*
void send_data_wire(int data_wire){
  Wire.beginTransmission(1); 
  Wire.write(data_wire);            
  Wire.endTransmission();
}
*/
