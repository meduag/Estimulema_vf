#include <SPI.h>
/* DAC values configurations */
#define DAC_OP_MODE 1
#define DAC_SS_PIN 10
#define STIM_MAXCH1 2048
#define STIM_MINCH1 2047
#define STIM_MAXCH2 2047
#define STIM_MINCH2 0
#define STIM_ZERO 2047
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
};

struct micros_times
{
	long time1; // ms
	long time2; // ms
	long t_now; // ms
};

int t = 100;
/* Initialize Structures*/
struct stimulation_parameters * data_sp; // stimulation values for CH1
struct micros_times * mtime; // values for time counters and therapy time
void setup()
{
	/* System initialization */
	// Communication
	Serial.begin(2000000);
	// DAC SS configuration Mode
	pinMode(DAC_SS_PIN, OUTPUT);
	// Dac configurations - LOW/0: Enable SPI communication, HIGH/1: Disable communication
	digitalWrite(DAC_SS_PIN, 1);
	SPI.begin();
	// Output configuration for signal stimulation on precarga
	zeroChannels();
	data_sp = (struct stimulation_parameters *) malloc(2 * sizeof(struct stimulation_parameters));
	InitializVal_struct(0);
	InitializVal_struct(1);
	mtime = (struct micros_times *) malloc(3 * sizeof(struct micros_times));
	InitializVal_Others(0);
	InitializVal_Others(1);
	InitializVal_Others(2);
}

void loop()
{
	/*data_sp[0].t_time_us = 1000000;
	data_sp[0].freq_T_us = 20000;
	data_sp[0].mA = STIM_MAXCH1 ;
	data_sp[0].pw_t_us = 500;
	data_sp[0].pw_t_us_r = 19000;
	bool whileciclo = true;
	while(whileciclo){
		stimulationWave_Square();
		whileciclo = false;
	}
	zeroChannels();
	delay(500);
	whileciclo = true;*/

	sendStimValue(0, 1,(STIM_ZERO + 1440));
}

void stimulationWave_Square()
{
	mtime[0].t_now = micros();
	mtime[0].time1 = micros();
	while (mtime[0].time1 <= mtime[0].t_now + data_sp[0].t_time_us)
	// therapy_duration
	{
		mtime[1].t_now = micros();
		mtime[1].time1 = micros();
		while (mtime[1].time1 <= mtime[1].t_now + data_sp[0].freq_T_us)
		// stimulation_parametersfreq
		{
			// ************************************** Positive pulse
			sendStimValue(0, 1,(STIM_ZERO + 2047));
			sendStimValue(1, 1,(STIM_ZERO + 1000));
			//sendStimValue(1, 1,(STIM_ZERO + data_sp[].mA));
			mtime[2].t_now = micros();
			mtime[2].time1 = micros();
			// aqui vou colocar la instrucion do DAC
			while (mtime[2].time1 <= mtime[2].t_now + data_sp[0].pw_t_us)
			// therapy_pw
			{
				mtime[2].time1 = micros(); // therapy_pw
			}
			// ************************************** Negative pulse
			// aqui vou colocar la instrucion do DAC
			sendStimValue(0, 1,(STIM_ZERO - 2047));
			sendStimValue(1, 1,(STIM_ZERO - 1000));
			//sendStimValue(1, 1,(STIM_ZERO - data_sp.mA));
			mtime[2].t_now = micros();
			mtime[2].time1 = micros();
			while (mtime[2].time1 <= mtime[2].t_now + data_sp[0].pw_t_us)
			// therapy_pw
			{
				mtime[2].time1 = micros(); // therapy_pw
			}
			// ************************************* Rest pulse - 0 estimulation
			sendStimValue(0, 1, STIM_ZERO);
			sendStimValue(1, 1, STIM_ZERO);
			mtime[2].t_now = micros();
			mtime[2].time1 = micros();
			// aqui vou colocar la instrucion for other things like recived data
			while (mtime[2].time1 <= mtime[2].t_now + data_sp[0].pw_t_us_r)
			// therapy_pw
			{
				mtime[2].time1 = micros(); // therapy_pw
			}
			mtime[1].time1 = micros(); // stimulation_parametersfreq
		}
		mtime[0].time1 = micros(); // therapy_duration
	}
	// Tempo final
	sendStimValue(0, 1,(STIM_ZERO + 20));
	/*
	mt.t_now = micros();
	mt.time1 = micros();
	while (mt.time1 <= mt.t_now + t.therapy_duration)
	// therapy_duration
	{
	mt.time1 = micros(); // therapy_duration
	}
	*/
}

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
}

void InitializVal_Others(int index)
{
	// For time micros
	mtime[index].time1 = 0;
	mtime[index].time2 = 0;
	mtime[index].t_now = 0;
}

void zeroChannels()
{
	sendStimValue(0, 1,(uint16_t) (STIM_ZERO + 20));
	sendStimValue(1, 1,(uint16_t) (STIM_ZERO + 20));
}
