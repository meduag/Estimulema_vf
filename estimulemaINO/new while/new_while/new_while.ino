#include <SPI.h>
#define DAC_SS_PIN 10

#define CH1_MAX_POS 1432
#define CH1_MIN_NEG 1330
#define STIM_ZERO 2047

unsigned long t_stop = micros() + 20e6;
unsigned long t_pnfl1 = 0, t_pnfl2 = 0;
bool lp1 = 1, ln1 = 1, lr1 = 1, ls1 = 1, lf1 = 1;
bool lp2 = 1, ln2 = 1, lr2 = 1, ls2 = 1, lf2 = 1;

// # depuration
//int time_c = 0, time_l = 0, yy = 200;
bool nada = true;


void setup() {
  Serial.begin(9600);

  SPI.begin();
  pinMode(DAC_SS_PIN, OUTPUT);
  digitalWrite(DAC_SS_PIN, 1);

  zeroChannels();

  delay(1000);
}

void loop(){
/*
  nada = false;
  while(time_c < t_stop && nada == true){

    if(lp){
      Serial.print("Positivo: ");
      lp = 0;
      lf = 1;
      t_pnfl = t_pnfl + 10;
      Serial.println(t_pnfl);
    }

    if(time_c >= t_pnfl && lp == 0 && ln == 1){
      Serial.print("Negativo: ");
      ln = 0;
      t_pnfl = t_pnfl + 10;
      Serial.println(t_pnfl);
      time_c +=  1;
    }

    if(time_c >= t_pnfl && ln == 0 && lr == 1){
      Serial.print("Rest: ");
      lr = 0;
      t_pnfl = t_pnfl + 20;
      Serial.println(t_pnfl);
      time_c +=  1;
    }

    if(time_c >= t_pnfl && lr == 0 && lf == 1){
      lf = 0;
      lp = 1;
      ln = 1;
      lr = 1;
      String str_var = "lf: " + String(lf) + "lp: " + String(lp) + "ln: " + String(ln);
      str_var = str_var + "lr: " + String(lr);
      Serial.print(str_var);
      Serial.print(" tp: ");
      Serial.println(t_pnfl);
      time_c +=  1;
    }

    time_c +=  1;
    Serial.println(time_c);
    delay(yy);

  }//fin while
*/
  t_stop = micros() + 60e6;
  //t_pnfl = 0;
  //lp = 1, ln = 1, lr = 1, ls = 1, lf = 1;
  //nada = true;

  /*
  while(micros() < t_stop && nada == true){
    if(lp){
    	Serial.println("Positivo");
     	lp = 0;
      lf = 1;
      //Serial.print(t_pnfl);
      //Serial.print(" - ");
      //Serial.print(micros());
     	t_pnfl = micros() + 1e6;
      //Serial.print(" - fin: ");
      //Serial.println(t_pnfl);
    }

    if(micros() >= t_pnfl && lp == 0 && ln == 1){
     	Serial.println("Negativo");
     	ln = 0;
      //Serial.print(t_pnfl);
      //Serial.print(" - ");
      //Serial.print(micros());
		  t_pnfl = micros() + 1e6;
      //Serial.print(" - fin: ");
      //Serial.println(t_pnfl);
    }

    if(micros() >= t_pnfl && ln == 0 && lr == 1){
     	Serial.println("Rest");
     	lr = 0;
      //Serial.print(t_pnfl);
      //Serial.print(" - ");
      //Serial.print(micros());
		  t_pnfl = micros() + 1e6;
      //Serial.print(" - fin: ");
      //Serial.println(t_pnfl);
    }

    if(micros() >= t_pnfl && lr == 0 && lf == 1){
     	lf = 0;
      lp = 1;
      ln = 1;
      lr = 1;
      //Serial.print("Fin miicros: ");
      //Serial.println(micros());
    }
  }//fin while
  */
  long pw1 = 1e6, pw2 = 2e5;

  while(micros() < t_stop && nada == true){

    //Para el lado possitivo
    if(lp1){
      Serial.print("Positivo1: ");
      Serial.println(micros());
      sendStimValue(0, 1, STIM_ZERO + CH1_MAX_POS);
      lp1 = 0;
      lf1 = 1;
      t_pnfl1 = micros() + pw1;
    }

    if(lp2){
      Serial.println("\t\tPositivo2");
      sendStimValue(1, 1, STIM_ZERO + CH1_MAX_POS);
      lp2 = 0;
      lf2 = 1;
      t_pnfl2 = micros() + pw2;
    }

    //Para el lado Negativo
    if(micros() >= t_pnfl1 && lp1 == 0 && ln1 == 1){
      Serial.print("Negativo1: ");
      Serial.println(micros());
      sendStimValue(0, 1, STIM_ZERO - CH1_MIN_NEG);
      ln1 = 0;
      t_pnfl1 = micros() + pw1;
    }

    if(micros() >= t_pnfl2 && lp2 == 0 && ln2 == 1){
      Serial.println("\t\tNegativo2");
      sendStimValue(1, 1, STIM_ZERO - CH1_MIN_NEG);
      ln2 = 0;
      t_pnfl2 = micros() + pw2;
    }

    //Para el lado Rest
    if(micros() >= t_pnfl1 && ln1 == 0 && lr1 == 1){
      Serial.print("Rest1: ");
      Serial.println(micros());
      sendStimValue(0, 1, STIM_ZERO);
      lr1 = 0;
      t_pnfl1 = micros() + pw1;
    }

    if(micros() >= t_pnfl2 && ln2 == 0 && lr2 == 1){
      Serial.println("\t\tRest2");
      sendStimValue(1, 1, STIM_ZERO);
      lr2 = 0;
      t_pnfl2 = micros() + pw2;
    }

    // reset final
    if(micros() >= t_pnfl1 && lr1 == 0 && lf1 == 1){
      lf1 = 0;
      lp1 = 1;
      ln1 = 1;
      lr1 = 1;
    }

    if(micros() >= t_pnfl2 && lr2 == 0 && lf2 == 1){
      lf2 = 0;
      lp2 = 1;
      ln2 = 1;
      lr2 = 1;
    }

  }//fin while

  if(nada){
    Serial.println("Terminou");
  }

  nada = false;

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

void zeroChannels() {
  //digitalWrite(RELAY_CH_1, 0);
  //digitalWrite(RELAY_CH_2, 0);
  sendStimValue(0, 1, (uint16_t) (STIM_ZERO));
  sendStimValue(1, 1, (uint16_t) (STIM_ZERO));
}