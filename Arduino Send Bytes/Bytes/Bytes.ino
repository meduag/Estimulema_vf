String stim_parameters[16];//coloco 16 String para guardar los parametros de estimulacion de cada canal

///Caracter de separacion de datos, de inicio y fin de los mensajes
#define ini '<'
#define csp ';'

String val = "";
int timeOut = 100;
int c = 0;
String data = "";

void setup() {
  Serial.begin(57600);
  Serial.setTimeout(timeOut);
  Serial.println("Texte de comunicacion");
  delay(1000);
}

void loop() {
  readdata();
  //Serial.print("leyendo el puerto, contador: ");
  //c ++;
  //Serial.println(c);
}

void readdata() {
  if (Serial.available()) {
    data = Serial.readStringUntil('>');
    Serial.readString();//Tira o resto do buffer  - hace un flush forzado
    //data2 = Serial.readString();
    Serial.print("Data In: ");
    Serial.println(data);
    data = "<1;1;60M;1M;1M;26315;50;0"
    //data = "<S1;C1;T10000;F19000;P500;N2000;O2000;A10";
    //separa_datos(data);
  }
}

void separa_datos(String parameters) {
  int c = 0;
  val = "";
  int size_data = parameters.length();//Pego el valor del tamaño de la String de entrada
  Serial.print("Data size: ");
  Serial.println(size_data);
  delay(100);
  char char_buff[size_data]; //buffer parametros con el tamaño de DataIn
  parameters.toCharArray(char_buff, size_data);//transformo el String a Char array
  int i = 0; // Capturo el pirmer caracter
  char char_in = char_buff[i]; // inicio del mensaje

  /********************************************************************************/
  //Aqui separo el mensaje principal
  /********************************************************************************/
  if (char_in == ini) { //caracter inicial
    i = 1; // comiezo con el segundo mensaje
    char_in = char_buff[i];
    while (i < size_data) { //Caracter final
      if (char_in != csp)  {//Caracter separador
        val = val + char_in;//Concateno los caracteres
      } else {
        //Almacena cada valor sepado por el ";"
        stim_parameters[c] = val;
        val = "";
        c += 1;
      }
      i += 1;
      char_in = char_buff[i];
    }
    // para el ultimo valor
    stim_parameters[c] = val;
  }
  Serial.println("Termino de hacer la separacion de los datos");
  imprimir();
  delay(5000);
}

void imprimir() {
  for (int i = 0; i < 10; i++) {
    String dataV = "Valor" + String(i) + ": " + stim_parameters[i];
    Serial.println(dataV);
  }
}

