/* Square wave geration */
void stimulationWave_Square()
{
	mt.t_now = micros();
	mt.time1 = micros();
	while (mt.time1 <= mt.t_now + data_sp.therapy_duration)
	// therapy_duration
	{
		pt.t_now = micros();
		pt.time1 = micros();
		digitalWrite(RELAY_CH_1, 1); // LOW/0: precarga, HIGH/1: carga
		digitalWrite(RELAY_CH_2, 1); // LOW/0: precarga, HIGH/1: carga
		while (pt.time1 <= pt.t_now + data_sp.stimulation_parametersfreq)
		// stimulation_parametersfreq
		{
			// ************************************** Positive pulse
			sendStimValue(0, 1,(STIM_ZERO + data_sp.mA));
			sendStimValue(1, 1,(STIM_ZERO + data_sp.mA));
			pwt.t_now = micros();
			pwt.time1 = micros();
			// aqui vou colocar la instrucion do DAC
			while (pwt.time1 <= pwt.t_now + data_sp.therapy_pw)
			// therapy_pw
			{
				pwt.time1 = micros(); // therapy_pw
			}
			// ************************************** Negative pulse
			// aqui vou colocar la instrucion do DAC
			sendStimValue(0, 1,(STIM_ZERO - data_sp.mA));
			sendStimValue(1, 1,(STIM_ZERO - data_sp.mA));
			pwt.t_now = micros();
			pwt.time1 = micros();
			while (pwt.time1 <= pwt.t_now + data_sp.therapy_pw)
			// therapy_pw
			{
				pwt.time1 = micros(); // therapy_pw
			}
			// ************************************* Rest pulse - 0 estimulation
			sendStimValue(0, 1, STIM_ZERO);
			sendStimValue(1, 1, STIM_ZERO);
			pwt.t_now = micros();
			pwt.time1 = micros();
			// aqui vou colocar la instrucion for other things like recived data
			while (pwt.time1 <= pwt.t_now + data_sp.therapy_pw_r)
			// therapy_pw
			{
				pwt.time1 = micros(); // therapy_pw
			}
			pt.time1 = micros(); // stimulation_parametersfreq
		}
		mt.time1 = micros(); // therapy_duration
	}
	// Tempo final
	digitalWrite(RELAY_CH_1, 0); // LOW/0: precarga, HIGH/1: carga
	digitalWrite(RELAY_CH_2, 0); // LOW/0: precarga, HIGH/1: carga
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



//Teste para usar con if para la generacion de la onda
void stimulationWave_Square_if()
{
	bool loop_f = false;
	bool loop_pw = false;
	bool cont1 = false, cont2 = false;
	mtime[0].t_now = micros();
	mtime[0].time1 = micros();
	mtime[1].t_now = mtime[0].time1;
	mtime[2].t_now = mtime[0].time1;
	while (mtime[0].time1 <= mtime[0].t_now + data_sp[0].t_time_us)
	{
		if (mtime[0].time1 <= mtime[1].t_now + data_sp[0].freq_T_us)
		{
			loop_f = true;
			continue;
		}
		else
		{
			mtime[1].t_now = mtime[0].time1;
			loop_f = false;
		}

		if(mtime[0].time1 <= mtime[2].t_now + data_sp[0].pw_t_us && loop_f == true && loop_pw = false)
		{
			if(cont1 == false)
			{
				sendStimValue(0, 1,(STIM_ZERO + data_sp[0].mA));
				cont1 = true;
			}
			continue;
		}
		else
		{
			if(cont2 == false)
			{
				mtime[2].t_now = mtime[0].time1;
				loop_pw = true;
				cont2 = true;
			}
			continue;
		}

		if(mtime[0].time1 <= mtime[2].t_now + data_sp[0].pw_t_us && loop_pw = true)
		{
			sendStimValue(0, 1,(STIM_ZERO - data_sp[0].mA));
		}
		mtime[0].time1 = micros();
	}
}