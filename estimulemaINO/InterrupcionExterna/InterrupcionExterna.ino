
#define PinInt 23  //Pin de interrupcion 23 Teensy, 2,3 Uno
#define ledPin1 13 //Pin que indica entro en el loop
#define ledPin2 8 //Pin que indica entro en la funcion causada por la interrupcion

int c = 0;
bool loops = 1;

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, LOW);
  pinMode(PinInt, INPUT); // sets the digital pin as output
  Serial.begin(2000000);
  attachInterrupt(PinInt, interrupcion, RISING); //Configutacion interrrupcion interna Master
}

void loop() {
  parpadear(loops);
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////            Interrupcion externa       //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
void interrupcion() {
  Serial.println("Interrupcion activada");
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  loops = 0;
  delay(100);
  digitalWrite(ledPin2, LOW);
}

void parpadear(bool loopss) {
  if (loopss) {
    digitalWrite(ledPin1, true);
    delay(500);
    digitalWrite(ledPin1, false);
    delay(500);
  }
}

