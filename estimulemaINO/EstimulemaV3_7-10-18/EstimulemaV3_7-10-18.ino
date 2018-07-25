#include <SPI.h>

#define DAC_SS_PIN 10
#define RELAY_CH_1 6
#define RELAY_CH_2 7
#define Pin_Sync_Data 8
#define Pin_signal_Control 14
#define Pin_Inter 23 
#define Gpio5 8
#define BuzzerPin 9


// Valores originales
#define CH1_MAX_POS 1883
#define CH1_MIN_NEG 1842
#define CH2_MAX_POS 2047
#define CH2_MIN_NEG 2047

#define STIM_ZERO 2047
#define fin '>'

#define pwrc 2e6 // time of rest used in tests rh and cr
//#define pwrc 10000 // time of rest used in tests rh and cr

// For data in
String data_in; 
unsigned int rh = 0, cr = 0, s_c = 1, upd = 0;
unsigned int ts = 0, freq = 0, lim_ma = 0, lim_pw = 0, pw_b = 50;
unsigned long pw = 0, pw_r = 0; 
unsigned long min_t = 60e6; // multiplicador tempo para min a us

bool loop_s = true;

struct stimulation_parameters{
  unsigned int ch_act;
  unsigned long tn; // in useconds
  unsigned long tf;
  unsigned long r;
  unsigned int ma; // Hz converted on Period T in us
};

struct stimulation_parameters * data_sp; // stimulation values for CH1

void read_dataIn();
void split_functionality();
void rheobase_chronaxie(int mA_s, int t1, int t2);
void stimulation_training(bool ch1, bool ch2);
void InitializVal_struct(int ch);
void print_dataIn();
void sendStimValue(int address, int operation_mode, uint16_t value);
long val_ma(int Am, int limit);
void Stop_functions();
void zeroChannels();


void setup() {
  Serial.begin(2000000);
  Serial.setTimeout(1);
  Serial.flush();

  SPI.begin();

  pinMode(DAC_SS_PIN, OUTPUT);
  pinMode(Pin_Sync_Data, OUTPUT);
  pinMode(Pin_signal_Control, OUTPUT);
  pinMode(RELAY_CH_1, OUTPUT);
  pinMode(RELAY_CH_2, OUTPUT);

  pinMode(Pin_Inter, INPUT);

  pinMode(BuzzerPin, OUTPUT);
  digitalWrite(BuzzerPin, 0);

  digitalWrite(DAC_SS_PIN, 1);

  digitalWrite(Pin_Sync_Data, 0); // Configurar para hacer el rising en el pon de int
  digitalWrite(Pin_signal_Control, 0); // Configurar para hacer el rising en el pon de int


  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);

  attachInterrupt(Pin_Inter, Stop_functions, RISING);

  data_sp = (struct stimulation_parameters *) malloc(4 * sizeof(struct stimulation_parameters));
  //Para el canal 1 y 2 mas dos estructuras temporales la 2 y 3
  for(int i=0; i<4; i++){
    InitializVal_struct(1);
  }

  zeroChannels();
  delay(1000);
}

void loop() {
  while (loop_s) {
    read_dataIn();
  }

  //separate and functionality
  split_functionality();

  zeroChannels();

  loop_s = true;
}

void read_dataIn() {
  if (Serial.available() != 0) {
    //-----------------------------------------------------------------/
    //Therapy time
    data_in = Serial.readStringUntil(fin);
    ts = data_in.toInt();
    //Serial.print("Ts: ");
    //Serial.println(ts);
    //Frequency
    data_in = Serial.readStringUntil(fin);
    freq = data_in.toInt();
    //Serial.print("Frequency: ");
    //Serial.println(freq);
    //Pulse width
    data_in = Serial.readStringUntil(fin);
    pw = data_in.toInt();
    //Serial.print("Pw: ");
    //Serial.println(pw);
    if(pw > 0){
      pw_r = int(1e6 / freq) - (pw * 2);
    }else{
      pw_r = 0;
    }
    //Serial.print("Pwr: ");
    //Serial.println(pw_r);

    //-----------------------------------------------------------------/
    /* for channel 1 */
    // Ton
    data_in = Serial.readStringUntil(fin);
    data_sp[0].tn = data_in.toInt();
    //Serial.print("Ton: ");
    //Serial.println(data_sp[0].tn);
    // Toff
    data_in = Serial.readStringUntil(fin);
    data_sp[0].tf = data_in.toInt();
    //Serial.print("Toff: ");
    //Serial.println(data_sp[0].tf);
    // Ramp
    data_in = Serial.readStringUntil(fin);
    data_sp[0].r = data_in.toInt();
    //Serial.print("Ramp: ");
    //Serial.println(data_sp[0].r);
    // Current
    data_in = Serial.readStringUntil(fin);
    data_sp[0].ma = data_in.toInt();
    //Serial.print("Current: ");
    //Serial.println(data_sp[0].ma);
    //-----------------------------------------------------------------/
    /* for channel 2 */
    // Ton
    data_in = Serial.readStringUntil(fin);
    data_sp[1].tn = data_in.toInt();
    // Toff
    data_in = Serial.readStringUntil(fin);
    data_sp[1].tf = data_in.toInt();
    // Ramp
    data_in = Serial.readStringUntil(fin);
    data_sp[1].r = data_in.toInt();
    // Current
    data_in = Serial.readStringUntil(fin);
    data_sp[1].ma = data_in.toInt();
    //-----------------------------------------------------------------/
    /* for tests */
    // for Rheobase limit
    data_in = Serial.readStringUntil(fin);
    lim_ma = data_in.toInt();
    // for Cronaxe limit
    data_in = Serial.readStringUntil(fin);
    lim_pw = data_in.toInt(); 
    // for Rheobase
    data_in = Serial.readStringUntil(fin);
    rh = data_in.toInt();
    // for Cronaxe
    data_in = Serial.readStringUntil(fin);
    cr = data_in.toInt();
    //-----------------------------------------------------------------/
    /* for Activations */
    // Channels 1 e 2
    data_in = Serial.readStringUntil(fin);
    data_sp[0].ch_act = data_in.toInt();
    //Serial.print("Ch1 activation: ");
    //Serial.println(data_sp[0].ch_act);
    // for Cronaxe
    data_in = Serial.readStringUntil(fin);
    data_sp[1].ch_act = data_in.toInt();

    data_in = Serial.readStringUntil(fin);
    s_c = data_in.toInt();
    //Serial.print("Stop control: ");
    //Serial.println(s_c);

    data_in = Serial.readStringUntil(fin);
    upd = data_in.toInt();
    //Serial.print("Update data: ");
    //Serial.println(upd);

    //Serial.println("Limpia buffer");
    Serial.readString();//limpia buffer 
    Serial.flush();//espera hasta el ultimo byte de In Out 

    //Serial.println("Terminou de la separacion de datos");

    //Serial.println("Multiplicando valores");
    for(int i=0; i<=1; i++){ //Pasa los datos a las estructuras
      data_sp[i].tn = data_sp[i].tn * 1e6;
      /*Serial.print("Ton ");
      Serial.print(i);
      Serial.print(" :");
      Serial.println(data_sp[0].tn);*/
      data_sp[i].tf = data_sp[i].tf * 1e6;
      /*Serial.print("Toff ");
      Serial.print(i);
      Serial.print(" :");
      Serial.println(data_sp[0].tf);*/
      data_sp[i].r = data_sp[i].r * 1e6;
      /*Serial.print("Ramp ");
      Serial.print(i);
      Serial.print(" :");
      Serial.println(data_sp[0].r);*/
    }

    //Serial.print("Accionando para de emergencia: ");
    if(s_c){
      //Serial.println("Negativo");
      loop_s = false;
    }else{ //Solo si activa el stop control
      //Serial.println("Positivo");
      digitalWrite(Pin_signal_Control, 0);
      zeroChannels();
      loop_s = true;
    }

    //Serial.println("pasando datos a las estructuras temporales");
    if(upd == 0){ //Solo la primera vez funciona
      data_sp[2].tn = data_sp[0].tn;
      data_sp[2].tf = data_sp[0].tf;
      data_sp[2].r = data_sp[0].r;
      data_sp[2].ma = data_sp[0].ma;

      data_sp[3].tn = data_sp[1].tn;
      data_sp[3].tf = data_sp[1].tf;
      data_sp[3].r = data_sp[1].r;
      data_sp[3].ma = data_sp[1].ma;
    }

    if(data_sp[0].ch_act == 0 && data_sp[1].ch_act == 0  && rh == 0 && cr == 0){
      loop_s = true; // reinicio el loop hasta que lleguen nuevos datos
      Serial.println("Mensaje corrupto - Intente nuevamente ");
    }else if(loop_s == false){
      //print_dataIn();
      //Serial.println("Parece que todo está bien - Saliendo de la lectura de datos");  
    }
  }// fin if serial avaliable
}// fin function

void split_functionality(){
  
  if(rh && s_c){
    //Serial.println("Start Rheobase");
    //Serial.println("Inicia captura dados Reobase");
    unsigned int ma = 0; // count the variable value of ma
    unsigned long ppw = 5e5; // fixed value of pw in rheobase test 500ms
    lim_ma = lim_ma + 1;
    digitalWrite(Pin_Sync_Data, 1);
    Serial.print("s0;");
    Serial.print(ma);
    Serial.print(";");
    Serial.println(micros());
    delay(2000);
    ma = 1;
    while (ma < lim_ma && s_c){
      Serial.print("s0;");
      Serial.print(ma);
      Serial.println(";0");
      rheobase_chronaxie(ma, ppw, pwrc);
      //print_dataIn();
      ma +=1;
      if(ma >= lim_ma && s_c){
        digitalWrite(RELAY_CH_1, 0); // termina la estimulacion en la carga
        digitalWrite(Pin_Sync_Data, 0);
        Serial.print("s0;");
        Serial.print(ma - 1);
        Serial.print(";");
        Serial.println(micros());
        rh = 0;
      }
    } // end while limit ma
  } // and if rh
  
  if(cr && s_c){
    //Serial.println("Start Chronaxie");
    //Serial.println("Inicia captura dados Cronaxia");
    digitalWrite(Pin_Sync_Data, 1); //Inicia a captura

    unsigned int pw_c = 0; // count the variable value of pw
    lim_pw = lim_pw + 1;
    Serial.print("s0;");
    Serial.print(pw_c);
    Serial.print(";");
    Serial.println(micros());
    delay(2000);
    pw_c = pw_b;
    while(pw_c < lim_pw && s_c) {
      Serial.print("s0;");
      Serial.print(pw_c);
      Serial.println(";0");
      rheobase_chronaxie(lim_ma, pw_c, pwrc);
      pw_c += pw_b;
      if(pw_c >= lim_pw && s_c){
        digitalWrite(RELAY_CH_1, 0); // termina la estimulacion en la carga
        digitalWrite(Pin_Sync_Data, 0);
        Serial.print("s0;");
        Serial.print(pw_c - pw_b);
        Serial.print(";");
        Serial.println(micros());
        cr = 0;
      }
    } // Fin while limit pw
  } // end if cr

  
  // for training stimulation
  if(data_sp[0].ch_act && data_sp[1].ch_act == 0 && s_c){
    Serial.println("Solo canal 1");
    stimulation_training(1,0);
  }else if(data_sp[0].ch_act == 0 && data_sp[1].ch_act){
    Serial.println("Solo canal 2");
    stimulation_training(0,1);
  }else if(data_sp[0].ch_act && data_sp[1].ch_act && s_c){
    Serial.println("Canal 1 e 2");
    stimulation_training(1,1);
  }
}// end function


void rheobase_chronaxie(int mA_s, int t1, int t2) {
  /*Serial.print("Current: ");
  Serial.print(mA_s);
  Serial.print("\tT1: ");
  Serial.print(t1);
  Serial.print("\tT2: ");
  Serial.print(t2);
  Serial.print("\tStop control: ");
  Serial.println(s_c);*/

  unsigned long t_stop = 0; // tiempo final
  digitalWrite(RELAY_CH_1, 1); // habilita canal 1 para salida
  digitalWrite(Pin_signal_Control, 1); // enable accel data capture
  sendStimValue(0, 1, STIM_ZERO + val_ma(mA_s, CH1_MAX_POS)); // star stimulation
  Serial.print("s1;");
  Serial.print(mA_s);
  Serial.print(";");
  Serial.println(micros()); // Initial value on micros
  t_stop = micros() + t1;
  while (micros() <= t_stop && s_c) {
    read_dataIn();
  }
  Serial.print("s1;");
  Serial.print(mA_s);
  Serial.print(";");
  Serial.println(micros());
  sendStimValue(0, 1, STIM_ZERO - val_ma(mA_s, CH1_MIN_NEG));
  Serial.print("s2;");
  Serial.print(mA_s);
  Serial.print(";");
  Serial.println(micros());
  t_stop = micros() + t1;
  while (micros() <= t_stop && s_c) {
    read_dataIn();
  }
  Serial.print("s2;");
  Serial.print(mA_s);
  Serial.print(";");
  Serial.println(micros());
  sendStimValue(0, 1, STIM_ZERO + 10);
  Serial.print("s0;");
  Serial.print(mA_s);
  Serial.println(";0");
  digitalWrite(Pin_signal_Control, 0);
  t_stop = micros() + t2;
  while (micros() <= t_stop && s_c) {
    read_dataIn();
  }
  //Serial.println("Salió ----------------------------------------------------");
}

void stimulation_training(int ch1, int ch2){
  
  //for signal control
  unsigned long t_stop = 0, t_ini_temp = 0;
  unsigned long t_pnfl1 = 0, t_pnfl2 = 0;
  bool lp1 = 1, ln1 = 1, lr1 = 1, lf1 = 1;
  bool lp2 = 1, ln2 = 1, lr2 = 1, lf2 = 1;

  bool tn1 = 1;
  bool tn2 = 1;
  unsigned long t_tn1 = 0, t_tf1 = 0;
  unsigned long t_tn2 = 0, t_tf2 = 0;

  unsigned long t_ts_temp = 0, t_ts_cal = 0, t_ts_cal_temp = 0;

  unsigned long seg10 = 0;

  // print_dataIn();

  //Serial.print("micros: ");
  //Serial.println(micros());

  //digitalWrite(BuzzerPin, 1);

  Serial.println(F("Entro en el ts"));
  //Inicializo variables
  t_ts_cal = ts * min_t;
  t_stop = micros() + t_ts_cal;
  t_tn1 = micros() + data_sp[0].tn;
  t_tn2 = micros() + data_sp[1].tn;
  t_ts_temp = micros();
  t_ini_temp = t_ts_temp; // para actualizar tiempo de terapia

  //Para testar seg en el while de ts
  seg10 = 1e6 + micros();
  int cont_s = 0;

  //print_dataIn();

  /*Serial.print("Inicia con Ts: ");
  Serial.println(t_ts_temp);
  Serial.print("Termina con Ts: ");
  Serial.println(t_stop);*/

  /*
  Serial.print("Comienza con Ton: ");
  Serial.println(t_tn1);
  */

  while(t_ts_temp < t_stop && s_c){
    //Serial.println("Estoy adentro");

    // for Tn
    if(micros() >= t_tn1 && tn1 && upd == 0 && s_c && ch1){
      tn1 = 0;
      //Serial.print("Terminou o Tn: ");
      //Serial.println(micros());
      //Serial.print("Inicia o Tf: ");
      t_tf1 = micros() + data_sp[0].tf;
      //Serial.println(t_tf1);
      //digitalWrite(BuzzerPin, 0);
    }

    
    if(micros() >= t_tn2 && tn2 && upd == 0 && s_c && ch2){
      tn2 = 0;
      //Serial.print("Terminou o Tn2 -- 222: ");
      //Serial.println("Inicia o Tf -- 222: ");
      if(data_sp[1].tf > 0){
        t_tf2 = micros() + data_sp[1].tf;
      }
    }
    

    // for tf
    if(micros() >= t_tf1 && tn1 == 0 && upd == 0 && s_c && ch1){
      //Serial.print("Terminou o Tf: ");
      //Serial.println(micros());
      //Serial.print("Inicia o Tn: ");
      t_tn1 = micros() + data_sp[0].tn;
      tn1 = 1;
      //Serial.println(t_tn1);
      //digitalWrite(BuzzerPin, 1);
    }

    
    if(micros() >= t_tf2 && tn2 == 0 && upd == 0 && s_c && ch2){
      //Serial.print("Terminou o Tf -- 222: ");
      //Serial.println("Inicia o Tn -- 222: ");
      t_tn2 = micros() + data_sp[1].tn;
      tn2 = 1;
    }
    

    
    //Para el lado possitivo
    if(micros() <= t_tn1 && lp1 && s_c && upd == 0 && ch1){
      //Serial.print("Positivo: ");
      //Serial.println(micros());
      digitalWrite(RELAY_CH_1, 1);
      sendStimValue(0, 1, STIM_ZERO + val_ma(data_sp[0].ma, CH1_MAX_POS));
      lp1 = 0;
      lf1 = 1;
      t_pnfl1 = micros() + pw;
    }

    
    if(micros() <= t_tn2 && lp2 && s_c && upd == 0 && ch2){
      //Serial.println("\t\tPositivo2");
      digitalWrite(RELAY_CH_2, 1);
      sendStimValue(1, 1, STIM_ZERO + val_ma(data_sp[1].ma, CH2_MAX_POS));
      lp2 = 0;
      lf2 = 1;
      t_pnfl2 = micros() + pw;
    }
    

    //Para el lado Negativo
    if(micros() <= t_tn1 && micros() >= t_pnfl1 && lp1 == 0 && ln1 && s_c && upd == 0 && ch1){
      //Serial.print("Negativo: ");
      //Serial.println(micros());
      sendStimValue(0, 1, STIM_ZERO - val_ma(data_sp[0].ma, CH1_MIN_NEG));
      ln1 = 0;
      t_pnfl1 = micros() + pw;
    }

    
    if(micros() <= t_tn2 && micros() >= t_pnfl2 && lp2 == 0 && ln2 && s_c && upd == 0 && ch2){
      //Serial.println("\t\tNegativo2");
      sendStimValue(1, 1, STIM_ZERO - val_ma(data_sp[1].ma, CH2_MIN_NEG));
      ln2 = 0;
      t_pnfl2 = micros() + pw;
    }
    

    //Para el lado Rest
    if(micros() <= t_tn1 && micros() >= t_pnfl1 && ln1 == 0 && lr1 && s_c && upd == 0 && ch1){
      //Serial.print("Rest: ");
      //Serial.println(micros());
      //digitalWrite(RELAY_CH_1, 0);
      sendStimValue(0, 1, STIM_ZERO);
      lr1 = 0;
      t_pnfl1 = micros() + pw_r;
    }

    
    if(micros() <= t_tn2 && micros() >= t_pnfl2 && ln2 == 0 && lr2 && s_c && upd == 0 && ch2){
      //Serial.println("\t\tRest2");
      //digitalWrite(RELAY_CH_2, 0);
      sendStimValue(1, 1, STIM_ZERO);
      lr2 = 0;
      t_pnfl2 = micros() + pw_r;
    }
    

    // reset final
    if(micros() <= t_tn1 && micros() >= t_pnfl1 && lr1 == 0 && lf1 && s_c && upd == 0 && ch1){
      //Serial.println("Final rest");
      //digitalWrite(RELAY_CH_1, 0);
      //sendStimValue(0, 1, STIM_ZERO+ 10);
      lf1 = 0;
      lp1 = 1;
      ln1 = 1;
      lr1 = 1;
    }

    
    if(micros() <= t_tn2 && micros() >= t_pnfl2 && lr2 == 0 && lf2 && s_c && upd == 0 && ch2){
      //digitalWrite(RELAY_CH_2, 0);
      //sendStimValue(1, 1, STIM_ZERO + 10);
      lf2 = 0;
      lp2 = 1;
      ln2 = 1;
      lr2 = 1;
    }
    
    
    if(micros() >= seg10){
      Serial.print(cont_s);
      Serial.println(" s");
      cont_s += 1;
      seg10 = 1e6 + micros();
    }
    
    //Serial.flush();
    read_dataIn();
    

    if(upd){ //si upd = 1 há atualização
      //Serial.println("Entro para actualizar los datos");

      sendStimValue(0, 1, STIM_ZERO + 10);
      sendStimValue(1, 1, STIM_ZERO + 10);


      // Para actualizar el valor de la terapia o del treinamiento
      t_ts_cal_temp = ts * min_t; // calcula el nuevo valor de t_stop tempralmente
      if(t_ts_cal != t_ts_cal_temp){
        //Serial.println("Actualiza tiempo de ts");
        if(t_ts_cal < t_ts_cal_temp){
          t_stop = t_ini_temp + t_ts_cal_temp;
        } 
        if((t_ts_temp - t_ini_temp) < (t_ini_temp + t_ts_cal_temp)) {
          t_stop = t_ini_temp + t_ts_cal_temp;
        } else{
          s_c = 0;
        }
      }

      //Activar canales
      if(data_sp[0].ma == 0){
        //Serial.println("Deshabilita canal 1");
        ch1 = 0;
      }else if(data_sp[0].ch_act && data_sp[0].ma > 0){
        //Serial.println("Habilita canal 1");
        ch1 = 1;
      }

      if(data_sp[1].ma == 0){
        //Serial.println("Deshabilita canal 2");
        ch2 = 0;
      }else if(data_sp[1].ch_act && data_sp[1].ma > 0){
        //Serial.println("Habilita canal 1");
        ch2 = 1;
      }
      
      //termina estimulacion
      if(data_sp[0].ma == 0 && data_sp[1].ma == 0){
        Serial.println("Parada de emergencia");
        s_c = 0; 
        zeroChannels();
      }
      
      //Actualizacion de corriente
      if(data_sp[0].ma != data_sp[2].ma){
        //Serial.println("Actualiza mA ch1");
        if (lp1 == 0 && ln1 && lf1){
          sendStimValue(0, 1, STIM_ZERO + val_ma(data_sp[0].ma, CH1_MAX_POS));
        } else if (ln1 == 0 && lf1){
          sendStimValue(0, 1, STIM_ZERO - val_ma(data_sp[0].ma, CH1_MIN_NEG));
        }
        data_sp[2].ma = data_sp[0].ma;
      }

      
      if(data_sp[1].ma != data_sp[3].ma){
        //Serial.println("Actualiza mA ch2");
        if (lp1 == 0 && ln1 && lf1){
          sendStimValue(1, 1, STIM_ZERO + val_ma(data_sp[1].ma, CH2_MAX_POS));
        } else if (ln1 == 0 && lf1){
          sendStimValue(1, 1, STIM_ZERO - val_ma(data_sp[1].ma, CH2_MIN_NEG));
        }
        data_sp[3].ma = data_sp[1].ma;
      }
      
      upd = 0; // desactivo la actualizacion
    } // Fin update

  t_ts_temp = micros(); //update micros
}//fin while uppdate

  Serial.print("Salió del while treinamento - terapía: ");
  Serial.println(micros());

  //Termina estimulacion]
  //zeroChannels();
  //loop_s = true; // Habilita recepcação

}


void InitializVal_struct(int ch){
  // For data of stimulation parameters
  data_sp[ch].ch_act = 0;
  data_sp[ch].tn = 0;
  data_sp[ch].tf = 0;
  data_sp[ch].r = 0;
  data_sp[ch].ma = 0;
}

void print_dataIn(){
  String msg = "";

  msg = String(ts) + fin + String(freq) + fin + String(pw) + fin ;
  //ch1
  msg = msg + String(data_sp[0].tn) + fin + String(data_sp[0].tf) + fin + String(data_sp[0].r) + fin + String(data_sp[0].ma);
  //ch1
  msg = msg + fin + String(data_sp[1].tn) + fin + String(data_sp[1].tf) + fin + String(data_sp[1].r) + fin + String(data_sp[1].ma);
  //limits
  msg = msg + fin + String(lim_ma) + fin + String(lim_pw) + fin;
  //activation
  msg = msg + String(rh) + fin + String(cr) + fin + String(data_sp[0].ch_act) + fin + String(data_sp[1].ch_act) + fin;
  //stop
  msg = msg + String(s_c) + fin + String(upd) + "> -- " + String(pw_r);

  Serial.println(msg);
  Serial.print("");
  Serial.print("");
  Serial.print("");
  Serial.print("");
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
  //Serial.println("Interrupcion activada");
  //Serial.println("Para captura de datos por interrupcion");
  s_c = 0;
  digitalWrite(Pin_Sync_Data, 0);
  digitalWrite(Pin_signal_Control, 0);
}

void zeroChannels() {
  digitalWrite(RELAY_CH_1, 0);
  digitalWrite(RELAY_CH_2, 0);
  sendStimValue(0, 1, (uint16_t) (STIM_ZERO + 10));
  sendStimValue(1, 1, (uint16_t) (STIM_ZERO + 10));
}
