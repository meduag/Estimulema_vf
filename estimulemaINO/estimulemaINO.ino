// Anotations
// Dac configurations - LOW/0: Enable SPI communication, HIGH/1: Disable communication
// For Relay control - LOW/0: precarga, HIGH/1: carga
// all values are in useconds
#include <SPI.h>
/* DAC values configurations */
#define DAC_OP_MODE 1
#define DAC_SS_PIN 10
/* Hardware pins definitions */
#define RELAY_CH_1 6
/* rele del canal 1 */
#define RELAY_CH_2 7
/* rele del canal 2 */
#define BuzzerPin 9
/* rele del canal 2 */
#define STOP_BUTTON 23
/* Emergency Button working with external nterruption*/
#define SERIAL_TIMEOUT 0
/* TimeOut value for serial communication */
/* Stimulation user configuration */
/* manual configurationl */
#define STIM_MAXCH1 3410
#define STIM_MINCH1 684
#define STIM_MAXCH2 3410
#define STIM_MINCH2 684
#define STIM_ZERO 2047
#define INTERPHASE_DELAY 0
/* us (offset de 30us)  */
#define time_us 1000000
/* us                   */
#define BUFFER_SIZE 10
/* Global variables              */
/* for testes*/
int teste = 1;
/* Data for Serial communication and data incoming */
String data;
String dataA;
int size_data;
boolean loop_ctrl = true;
String charStr = "";
int activation_ch[] =
{
	0, 0
};

boolean stim_start = false;
/* constant charracters, ini = inizialite string for data in, csp = separation character */
#define ini '<'
#define csp ';'
#define fin '>'
#define iniA '('
#define finA ')'
/* Estructuras */
struct stimulation_parameters
{
	char ch_id;
	long t_time_us; // in useconds
	long t_on_us;
	long t_off_us;
	long freq_T_us; // Hz converted on Period T in us
	long pw_t_us;
	long pw_t_us_r;
	int mA;
	long time_btw_chs;
};

struct micros_times
{
	long time1; // ms
	long time2; // ms
	long t_now; // ms
};

/* Initialize Structures*/
struct stimulation_parameters * data_sp; // stimulation values for CH1
struct micros_times * mtime; // values for time counters and therapy time
// auxiliar variables
// Functions declarations
void zeroChannels();
void read_dataIn();
void separate_data();
void update_individual_data(int ch, int uid_i);
int while_udi(int uid_i);
void update_group_data();
void while_ugd_v2(String data_channel, int ch);
void while_ugd(String val, int ch, int ugd_par);
void sendStimValue(int address, int operation_mode, uint16_t value);
void printData();
void InitializVal_struct(int ch);
void InitializVal_Others();
void stimulationWave_Square();
/* First run */
void setup()
{
	/* System initialization */
	// Communication
	Serial.begin(2000000);
	Serial.setTimeout(SERIAL_TIMEOUT);
	// DAC SS configuration Mode
	pinMode(DAC_SS_PIN, OUTPUT);
	// Dac configurations - LOW/0: Enable SPI communication, HIGH/1: Disable communication
	digitalWrite(DAC_SS_PIN, 1);
	// Buzzer
	pinMode(BuzzerPin, OUTPUT);
	digitalWrite(BuzzerPin, 0);
	SPI.begin();
	// Relays configuration Mode
	pinMode(RELAY_CH_1, OUTPUT);
	pinMode(RELAY_CH_2, OUTPUT);
	// Relays configurations - LOW: Enable channel output, HIGH: Enable output for precharge resistor
	digitalWrite(RELAY_CH_1, 0); // LOW/0: precarga, HIGH/1: carga
	digitalWrite(RELAY_CH_2, 0); // LOW/0: precarga, HIGH/1: carga
	// Output configuration for signal stimulation on precarga
	zeroChannels();
	data_sp = (struct stimulation_parameters *) malloc(2 * sizeof(struct stimulation_parameters));
	InitializVal_struct(0);
	InitializVal_struct(1);
	mtime = (struct micros_times *) malloc(5 * sizeof(struct micros_times));
	InitializVal_Others(0); // for time stimulation session
	InitializVal_Others(1); // for time (period) frequency
	InitializVal_Others(2); // for time pw signal
	InitializVal_Others(3); // for time on stimulation
	InitializVal_Others(4); // for time off stimulation
	// printData(1);
	// printData(2);
	// printData(3);
	// Predetermined values para testes
	/*if (teste == 1)
	{
	data_sp.freq = 300; // Hz
	data_sp.ampli = 5; // milli amps
	data_sp.therapy_duration = 1;
	data_sp.therapy_pw = 500;
	data_sp.ampli = 100;
	// configuration of initial values
	data_sp.therapy_duration = data_sp.therapy_duration * mt.multi_us; // in us
	data_sp.stimulation_parametersfreq = mt.multi_us / data_sp.freq; // period time in us
	data_sp.therapy_pw_r = data_sp.stimulation_parametersfreq - (data_sp.therapy_pw * 2);
	// Este valor deberia ser pasado directamente por la GUI
	data_sp.mA = map(data_sp.ampli, 0, 100, 0, 2047);
	}*/
	// Serial.println("Configurations ends");
}

/* Eternal loop */
void loop()
{
	read_dataIn();
	/*data_sp[0].t_time_us = 1000000;
	data_sp[0].freq_T_us = 20000;
	data_sp[0].mA = STIM_MAXCH1;
	data_sp[0].pw_t_us = 500;
	data_sp[0].pw_t_us_r = 19000;
	*/
	while (stim_start)
	{
		if (activation_ch[0] == 1 && activation_ch[1] == 0)
		{
			Serial.println("estimulando en el primer canal");
			stimulationWave_Square(0, true); // first argument: channel, second: group/indv Activation
		}
		else
			if (activation_ch[0] == 0 && activation_ch[1] == 1)
			{
				Serial.println("estimulando en el segundo canal");
				stimulationWave_Square(1, true); // first argument: channel, second: group/indv Activation
			}
		else
			if (activation_ch[0] == 1 && activation_ch[1] == 1)
			{
				// colocar aqui cuando son secuenciales o son dependientes
				// secuenciales se coloca una activacion e despues la otra tal vez una nueva funcion
				Serial.println("estimulando en los dos canales");
				stimulationWave_Square(0, false); // first argument: channel, second: group/indv Activation
			}
	}
}

/****************************************************************************************/
/************************************ System Functions **********************************/
/* Data reception */
void read_dataIn()
{
	// data = (data*) malloc(sizeof(data));
	if (Serial.available())
	{
		data = Serial.readStringUntil(fin);
		dataA = Serial.readStringUntil(finA);
		Serial.readString(); // Tira o resto do buffer  - hace un flush forzado
		//String dainIn_dataActivation = "Data in: " + data + " --- " + dataA;
		//Serial.println(dainIn_dataActivation);
		loop_ctrl = true;
		separate_data();
		// Calculate the rest pulse width
		data_sp[0].pw_t_us_r = data_sp[0].freq_T_us - (data_sp[0].pw_t_us * 2);
		data_sp[1].pw_t_us_r = data_sp[1].freq_T_us - (data_sp[1].pw_t_us * 2);
		separate_activation();
		printData(3);
	}
}

void separate_data()
{
	while (loop_ctrl == true)
	{
		/* control variables */
		String val = "";
		size_data = data.length();
		// Serial.println("longitud cadena: " + String(size_data));
		int i = 0; // Capturo el pirmer caracter
		/********************************************************************************/
		// Aqui separo el mensaje principal
		/********************************************************************************/
		if (data.charAt(i) == ini)
		{
			// Serial.println("Entró en el caracter de inicio - dato en grupo o individual");
			// charStr = data.charAt(1);
			// Serial.println("Siguiente caracter: " + charStr);
			switch (data.charAt(1))
			{
				// Serial.println("Parametro individual");
				case '0':
				// charStr = data.charAt(3);
				// Serial.println("Siguiente caracter: " + charStr);
				switch (data.charAt(3))
				{
					case '0':
						Serial.println("Parada obligatoria [3]");
						zeroChannels();
						loop_ctrl = false;
						stim_start = false;
						break;
					case '1':
					// Serial.println("Parametros cannal 1");
					update_individual_data(0, 5); // next chacracter
					// printData(1);
					loop_ctrl = false;
					stim_start = true;
					break;
					case '2':
					// Serial.println("Parametros cannal 2");
					// int gg = sizeof(struct stimulation_parameters);
					// Serial.println("size_of: " + String(gg));
					update_individual_data(1, 5);
					// printData(2);
					loop_ctrl = false;
					stim_start = true;
					break;
				}
				break;
				case '1':
				// Serial.println("Grupo de Parametros");
				// Serial.println("Siguiente caracter: " + data.charAt(1));
				update_group_data();
				// printData(3);
				loop_ctrl = false;
				stim_start = true;
				break;
				case '2':
					Serial.println("Parada obligatoria [1] dois canais");
					zeroChannels();
					loop_ctrl = false;
					stim_start = false;
					break;
				case '3':
					Serial.println("Parada obligatoria [1] canal 1");
					digitalWrite(RELAY_CH_1, 0);// LOW/0: precarga, HIGH/1: carga
					data_sp[0].mA = 0;
					loop_ctrl = false;
					stim_start = false;
					break;
				case '4':
					Serial.println("Parada obligatoria [1] canal 2");
					digitalWrite(RELAY_CH_2, 0);// LOW/0: precarga, HIGH/1: carga
					data_sp[1].mA = 0;
					loop_ctrl = false;
					stim_start = false;
					break;
			}
		}
		else
		{
			Serial.println("Mensaje corrompido");
			loop_ctrl = false;
			zeroChannels();
			// Alarme de mensaje corrompido colocar bip del buzzer aqui
		}
	}
}

// End separate_data
void update_individual_data(int ch, int uid_i) // sel channel, num counter
{
	// Serial.println("Funcion de separacion de parametros individuales para el canal: " + String(ch));
	switch (data.charAt(uid_i))
	{
		case 'T':
		// All values arrive in seconds and they needs
		// Serial.println("Time therapy");
		data_sp[ch].t_time_us = while_udi(uid_i + 1) * time_us;
		// Serial.println("Valor separado Terapia: " + String(data_sp[ch].t_time_us) + "us");
		break;
		case 'N':
		// Serial.println("T On");
		data_sp[ch].t_on_us = while_udi(uid_i + 1) * time_us;
		// Serial.println("Valor separado T on: " + String(data_sp[ch].t_on_us) + " us");
		break;
		case 'O':
		// Serial.println("T Off");
		data_sp[ch].t_off_us = while_udi(uid_i + 1) * time_us;
		// Serial.println("Valor separado T off: " + String(data_sp[ch].t_off_us) + " us");
		break;
		case 'F':
		// Serial.println("Frequencia");
		data_sp[ch].freq_T_us = while_udi(uid_i + 1); // Value already arrive in us
		// Serial.println("Valor separado frequencia: " + String(data_sp[ch].freq_T_us) + " us");
		break;
		case 'P':
		// Serial.println("Pulse whithd");
		data_sp[ch].pw_t_us = while_udi(uid_i + 1);
		// Serial.println("Valor separado largura de pulso: " + String(data_sp[ch].pw_t_us) + " us");
		break;
		case 'C':
		// Serial.println("Corrente");
		data_sp[ch].mA = while_udi(uid_i + 1);
		// Serial.println("Valor separado: corrente" + String(data_sp[ch].mA));
		break;
	}
}

int while_udi(int uid_i) // num counter of values
{
	String val = "";
	while (uid_i <= size_data)
	{
		val = val + data.charAt(uid_i); // Concateno los caracteres
		uid_i += 1;
	}
	// Serial.println("Valor final do while: " + val);
	return val.toInt();
}

void update_group_data()
{
	// data = "<1;0;60;10;10;20000;100;1>;
	String val = data.charAt(3);
	int ch = val.toInt();
	// Serial.println("canal de grupo: " + String(ch));
	val = "";
	int i = 5; // Start data
	int c = 0;
	int char_separador_data = 0;
	while (i <= size_data && char_separador_data == 0)
	{
		// Caracter final
		if (data.charAt(i) != csp && data.charAt(i) != 0)
		{
			// Caracter separador
			val = val + data.charAt(i); // Concateno los caracteres
			// Serial.println("Sequencial de val: " + val);
		}
		else
		{
			// salvo parametros para os dois canais
			// Serial.println("Temporal de val: " + val);
			if (ch == 0 || ch == 1)
			{
				// Serial.println("Separacion de datos de canales independientes");
				while_ugd(val, ch, c);
				// Serial.println("Valor de c: " + String(c));
				c += 1;
			}
			else
				if (ch == 2)
				{
					// Serial.println("Para separadcion de datos de dos canales");
					// <1;2;60;1;1;1000000;50;0:60;1;1;1000000;50;0>
					String data_CH1 = "";
					String data_CH2 = "";
					char_separador_data = data.indexOf(":", 20);
					// Serial.println("Index caracter separador: " + String(char_separador_data));
					data_CH1 = data.substring(5,(char_separador_data));
					data_CH2 = data.substring((char_separador_data + 1), size_data);
					// data_CH2 = "<" + data_CH2;
					// Serial.println("Cadena 1: " + data_CH1);
					// Serial.println("Cadena 2: " + data_CH2);
					while_ugd_v2(data_CH1, 0); // ch1
					while_ugd_v2(data_CH2, 1); // ch2
				}
			else
			{
				Serial.println("Sin opciones o datos, parada de emergencia");
				zeroChannels();
				char_separador_data = 1;
			}
			val = "";
		}
		// Serial.println("Valor de I: " + String(i) + " - Caracter: " + data.charAt(i));
		i += 1;
	}
	// Imprimo el ultimo valor sabioendo que i es mayor que el tamanho del vector de entrada
	if (i > size_data && char_separador_data == 0)
	{
		while_ugd(val, ch, c);
		// Serial.println("Valor de c: " + String(c));
	}
}

void while_ugd(String val, int ch, int ugd_par) // first argument: separate value, sel channel, num sel
{
	// Serial.println("Funcion de separacion de parametros grupales para el canal: " + String(ch));
	switch (ugd_par)
	{
		case 0:
		// Serial.println("T: " + val);
		data_sp[ch].t_time_us = (val.toInt()) * time_us;
		// Serial.println("Grupo - Valor separado Terapia: " + String(data_sp[ch].t_time_us) + " us");
		break;
		case 1:
		// Serial.println("Tn: " + val);
		data_sp[ch].t_on_us = (val.toInt()) * time_us;
		// Serial.println("Grupo - Valor separado T on: " + String(data_sp[ch].t_on_us) + " us");
		break;
		case 2:
		// Serial.println("To: " + val);
		data_sp[ch].t_off_us = (val.toInt()) * time_us;
		// Serial.println("Grupo - Valor separado T off: " + String(data_sp[ch].t_off_us) + " us");
		break;
		case 3:
		// Serial.println("F: " + val);
		data_sp[ch].freq_T_us = val.toInt(); // Value already arrive in us
		// Serial.println("Grupo - Valor separado Frequencia: " + String(data_sp[ch].freq_T_us) + " us");
		break;
		case 4:
		// Serial.println("Pw: " + val);
		data_sp[ch].pw_t_us = val.toInt();
		// Serial.println("Grupo - Valor separado Largura de pulso: " + String(data_sp[ch].pw_t_us) + " us");
		break;
		case 5:
		// Serial.println("C: " + val);
		data_sp[ch].mA = val.toInt();
		// Serial.println("Grupo - Valor separado: Corrente: " + String(data_sp[ch].mA));
		break;
	}
}

void while_ugd_v2(String data_channel, int ch) // first argument: data channel, second: sel channel
{
	String val = "";
	int i = 0;
	int c = 0;
	// Serial.println("Data de entrada: " + data_channel);
	int size_data_temp = data_channel.length() - 1;
	// Serial.println("size_data_temp: " + String(size_data_temp));
	while (i <= size_data_temp)
	{
		// Caracter final
		if (data_channel.charAt(i) != csp)
		{
			// Caracter separador
			val = val + data_channel.charAt(i); // Concateno los caracteres
			// Serial.println("Val concatenado: " + val + " -- Valor de i:" + String(i));
		}
		else
		{
			// Serial.println("Val concatenado: " + val);
			while_ugd(val, ch, c);
			c += 1;
			// Serial.println("Valor de C en la parte grupal: " + String(c));
			val = "";
			// Serial.println("Valor de i: " + String(i));
		}
		i += 1;
		if (i > size_data_temp && c == 5)
		{
			// Serial.println("--- Val concatenado: " + val);
			// Serial.println("--- Valor de C en la parte grupal: " + String(c));
			while_ugd(val, ch, c);
		}
	}
}

// function to separate data and information to activate channels and
void separate_activation()
{
	int i = 0;
	// Serial.println("Separar activacion");
	// Serial.println("char 0: " + String(dataA.charAt(i)));
	if (dataA.charAt(i) == iniA)
	{
		i = 1;
		// Serial.println("char 1: " + String(dataA.charAt(i)));
		switch (dataA.charAt(i))
		{
			case '1':
			// Serial.println("Canal 1 activado");
			activation_ch[0] = 1;
			activation_ch[1] = 0;
			break;
			case '2':
			// Serial.println("Canal 2 activado");
			activation_ch[0] = 0;
			activation_ch[1] = 1;
			break;
			case '3':
			// Serial.println("Canal 1 e 2 activados");
			activation_ch[0] = 1;
			activation_ch[1] = 1;
			break;
			default:
			// Serial.println("Canales desactivado");
			activation_ch[0] = 0;
			activation_ch[1] = 0;
		}
	}
}

// ########################################################################################### //
// ##############---------     functions for Wave generation
// ########################################################################################### //
/* Square wave geration */
void stimulationWave_Square(int ch, bool chA) // first argument: channel, second: group or indv channel activation
{
	mtime[0].t_now = micros();
	mtime[0].time1 = micros();
	while (mtime[0].time1 <= mtime[0].t_now + data_sp[ch].t_time_us)
	// Time estimulation session
	{
		mtime[3].t_now = micros();
		mtime[3].time1 = micros();
		while (mtime[3].time1 <= mtime[3].t_now + data_sp[ch].t_on_us)
		// Time on
		{
			mtime[1].t_now = micros();
			mtime[1].time1 = micros();
			while (mtime[1].time1 <= mtime[1].t_now + data_sp[ch].freq_T_us)
			// time (perios) for frequency
			{
				// ************************************** Positive pulse
				sel_channel_stimulate(1, ch, chA);
				// sendStimValue(0, 1,(STIM_ZERO + data_sp[0].mA));
				// sendStimValue(1, 1,(STIM_ZERO + data_sp[1].mA));
				mtime[2].t_now = micros();
				mtime[2].time1 = micros();
				while (mtime[2].time1 <= mtime[2].t_now + data_sp[ch].pw_t_us)
				// time pulse width for positive pulse
				{
					read_dataIn(); // read new data in
					mtime[2].time1 = micros(); // therapy_pw
				}
				// ************************************** Negative pulse
				// aqui vou colocar la instrucion do DAC
				sel_channel_stimulate(0, ch, chA);
				// sendStimValue(0, 1,(STIM_ZERO - data_sp[0].mA));
				// sendStimValue(1, 1,(STIM_ZERO - data_sp[1].mA));
				mtime[2].t_now = micros();
				mtime[2].time1 = micros();
				while (mtime[2].time1 <= mtime[2].t_now + data_sp[ch].pw_t_us)
				// time pulse width for negative pulse
				{
					read_dataIn(); // read new data in
					mtime[2].time1 = micros(); // pulse width - update value
				}
				// end pulse width
				// ************************************* Rest pulse - 0 estimulation
				zeroChannels();
				// sendStimValue(0, 1, STIM_ZERO);
				// sendStimValue(1, 1, STIM_ZERO);
				mtime[2].t_now = micros();
				mtime[2].time1 = micros();
				// aqui vou colocar la instrucion for other things like recived data
				while (mtime[2].time1 <= mtime[2].t_now + data_sp[ch].pw_t_us_r)
				// time pulse width rest
				{
					read_dataIn(); // read new data in
					mtime[2].time1 = micros(); // pulse width - update value
				}
				// end pulse width rest
				mtime[1].time1 = micros(); // time (period) frequency - update value
			}
			// end time (period) frequency
			mtime[3].time1 = micros(); // time on - update value
		}
		// end time on
		mtime[4].t_now = micros();
		mtime[4].time1 = micros();
		if (data_sp[ch].t_off_us > 0)
		{
			while (mtime[4].time1 <= mtime[4].t_now + data_sp[ch].t_off_us)
			// Time off
			{
				read_dataIn();
				mtime[4].time1 = micros(); // time off - update value
			}
		}
		// time off
		mtime[0].time1 = micros(); // time stimulation session - update value
	}
	// end time stimulation session
	zeroChannels();
	stim_start = false;
}

// end function square
// ########################################################################################### //
// ##############---------     functions for Send Data to DAC
// ########################################################################################### //
void sendStimValue(int address, int operation_mode, uint16_t value)
{
	/* for data on SPI communication */
	byte valueToWriteH = 0;
	byte valueToWriteL = 0;
	// Serial.print("Valor de entrada: ");
	// Serial.println(value);
	valueToWriteH = 0;
	valueToWriteL = 0;
	valueToWriteH = highByte(value);
	valueToWriteH = 0b00001111 & valueToWriteH;
	valueToWriteH = (address << 6) | (operation_mode << 4) | valueToWriteH;
	valueToWriteL = lowByte(value);
	// take the SS pin low to select the chip:
	digitalWrite(DAC_SS_PIN, LOW);
	// send in the address and value via SPI:
	SPI.transfer(valueToWriteH);
	SPI.transfer(valueToWriteL);
	// take the SS pin high to de-select the chip:
	digitalWrite(DAC_SS_PIN, HIGH);
}

void sel_channel_stimulate(int fase, int enable_ch, bool enable_ch_indv) // Identify the fase 0 for negative and 1 for possitive
{
	// To activate the channel 0 or 1
	if (enable_ch_indv == true)
	{
		//Serial.println("Activacion individual");
		Serial.println("Activacion para el canal: " + String(enable_ch));
		if (activation_ch[enable_ch] == 1 && fase == 1)
		{
			//Serial.println("Pulso positivo");
			digitalWrite(RELAY_CH_1, 1); // LOW/0: precarga, HIGH/1: carga
			sendStimValue(enable_ch, 1,(STIM_ZERO + data_sp[enable_ch].mA));
		}
		if (activation_ch[enable_ch] == 1 && fase == 0)
		{
			//Serial.println("Pulso negativo");
			digitalWrite(RELAY_CH_1, 1); // LOW/0: precarga, HIGH/1: carga
			sendStimValue(enable_ch, 1,(STIM_ZERO - data_sp[enable_ch].mA));
		}
	}
	else
	{
		// To activate all channels
		Serial.println("Activacion grupal");
		//Serial.println("Activacion para los dos canales");
		if (fase == 1)
		{
			//Serial.println("Pulso positivo");
			digitalWrite(RELAY_CH_1, 1); // LOW/0: precarga, HIGH/1: carga
			digitalWrite(RELAY_CH_2, 1); // LOW/0: precarga, HIGH/1: carga
			sendStimValue(0, 1,(STIM_ZERO + data_sp[0].mA));
			sendStimValue(1, 1,(STIM_ZERO + data_sp[1].mA));
		}
		else
			if (fase == 0)
			{
				//Serial.println("Pulso negativo");
				digitalWrite(RELAY_CH_1, 1); // LOW/0: precarga, HIGH/1: carga
				digitalWrite(RELAY_CH_2, 1); // LOW/0: precarga, HIGH/1: carga
				sendStimValue(0, 1,(STIM_ZERO - data_sp[0].mA));
				sendStimValue(1, 1,(STIM_ZERO - data_sp[1].mA));
			}
	}
}

// ########################################################################################### //
// ##############---------     functions for print data
// ########################################################################################### //
void printData(int printData)
{
	String val = "";
	String val2 = "";
	String val3 = "";
	if (printData == 1)
	{
		// Serial.println("Datos canal 1");
		val = "Terapia: " + String(data_sp[0].t_time_us) + " - Ton: " + String(data_sp[0].t_on_us) + " - Toff: " +
		String(data_sp[0].t_off_us) + " - Freq: " + String(data_sp[0].freq_T_us) + " - LarguraP: " +
		String(data_sp[0].pw_t_us) + " - Corrente: " + String(data_sp[0].mA) + " - tempoR" + String(data_sp[0].pw_t_us_r);
		Serial.println(val);
	}
	else
		if (printData == 2)
		{
			// Serial.println("Datos canal 2");
			val2 = "Terapia: " + String(data_sp[1].t_time_us) + " - Ton: " + String(data_sp[1].t_on_us) + " - Toff: " +
			String(data_sp[1].t_off_us) + " - Freq: " + String(data_sp[1].freq_T_us) + " - LarguraP: " +
			String(data_sp[1].pw_t_us) + " - Corrente: " + String(data_sp[1].mA) + " - tempoR" + String(data_sp[1].pw_t_us_r);
			Serial.println(val2);
		}
	else
	{
		// Serial.println("Datos canal 1 y 2");
		val = "Terapia: " + String(data_sp[0].t_time_us) + " - Ton: " + String(data_sp[0].t_on_us) + " - Toff: " +
		String(data_sp[0].t_off_us) + " - Freq: " + String(data_sp[0].freq_T_us) + " - LarguraP: " +
		String(data_sp[0].pw_t_us) + " - Corrente: " + String(data_sp[0].mA) + " - tempoR" + String(data_sp[0].pw_t_us_r);
		val2 = "Terapia: " + String(data_sp[1].t_time_us) + " - Ton: " + String(data_sp[1].t_on_us) + " - Toff: " +
		String(data_sp[1].t_off_us) + " - Freq: " + String(data_sp[1].freq_T_us) + " - LarguraP: " +
		String(data_sp[1].pw_t_us) + " - Corrente: " + String(data_sp[1].mA) + " - tempoR" + String(data_sp[1].pw_t_us_r);
		val3 = "Ch1: " + val + " --- " + "Ch2: " + val2;
		Serial.println(val3);
	}
}

// ########################################################################################### //
// ##############---------     functions for initialize variables
// ########################################################################################### //
void InitializVal_struct(int ch)
{
	// For data of stimulation parameters
	data_sp[ch].t_time_us = 0;
	data_sp[ch].t_on_us = 0;
	data_sp[ch].t_off_us = 0;
	data_sp[ch].freq_T_us = 0;
	data_sp[ch].pw_t_us = 0;
	data_sp[ch].pw_t_us = 0;
	data_sp[ch].mA = 0;
	data_sp[ch].time_btw_chs = 0;
	// For activaction channel
	activation_ch[0] = 0;
	activation_ch[1] = 0;
}

void InitializVal_Others(int index)
{
	// For time micros
	mtime[index].time1 = 0;
	mtime[index].time2 = 0;
	mtime[index].t_now = 0;
}

// ########################################################################################### //
// ##############---------     functions for zero channels
// ########################################################################################### //
void zeroChannels()
{
	digitalWrite(RELAY_CH_1, 0);// LOW/0: precarga, HIGH/1: carga
	digitalWrite(RELAY_CH_2, 0);// LOW/0: precarga, HIGH/1: carga
	sendStimValue(0, 1,(uint16_t) (STIM_ZERO + 20));
	sendStimValue(1, 1,(uint16_t) (STIM_ZERO + 20));
}
