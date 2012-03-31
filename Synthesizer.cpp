/*
 * Synthesizer.cpp
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#include "Synthesizer.h"

Synthesizer * Synthesizer::synthInstance = NULL;

Synthesizer::Synthesizer(EventBuffer & b, SynthGui & g): events(b), gui(g)
{
	unsigned int i;

	synthInstance = this;
	synthIsRunning = false;
	samplerate = 1;
	bufferLength = 0;
	jackCallbackLock = PTHREAD_MUTEX_INITIALIZER;
	samplerateChanged = false;
	bufferLengthChanged = false;

	oscillatorBuffer = NULL;
	lfoBuffer = NULL;

	mainVolume = 1;

	initJack();
	Oscillator::setSamplerate(samplerate);
	Oscillator::setBufferLength(bufferLength);

	oscillatorBuffer = new float[bufferLength];
	lfoBuffer = new float[bufferLength];

	lfo1frequencyType = FIXED;

	filter.setFrequency(MinLowpassFrequency);

	for (i = 0; i < POLYPHONY; i++) {
		oscillator1[i] = new MainOscillator(osc1parameters);
		lfo1[i] = new LowFrequencyOscillator(osc1parameters);
	}

	if (lfo1[POLYPHONY - 1] == NULL) {
		std::cout << "Problem in starting the synth: not enough free memory."
			<< std::endl;
	}
}

Synthesizer::~Synthesizer() {
	unsigned int i;

	if (synthIsRunning) {
		jack_client_close(jackClient);
	}
	delete [] oscillatorBuffer;
	delete [] lfoBuffer;

	for (i = 0; i < POLYPHONY; i++) {
		delete oscillator1[i];
		delete lfo1[i];
	}
}

bool Synthesizer::isActive()
{
	return synthIsRunning;
}

void Synthesizer::initJack()
{
	jack_options_t options = JackNoStartServer;
	jack_status_t status;
	jackClient = jack_client_open("maucisynth", options, &status);
	if (jackClient == NULL) {
		std::cout << "Problem in starting the synth. ";
		if (status & JackServerFailed) {
			std::cout << "Cannot connect to the JACK server.";
		}
		if (status & JackServerError) {
			std::cout << "Communication error with the JACK server.";
		}
		if (status & JackInitFailure) {
			std::cout << "Cannot initialize new JACK client.";
		}
		if (status & JackShmFailure) {
			std::cout << "Cannot access to shared memory.";
		}
		if (status & JackVersionError) {
			std::cout << "Client and server have different "
				"JACK protocol versions.";
		}
		std::cout << std::endl;
		return;
	}
	jack_set_process_callback (jackClient, Synthesizer::jackCallback, 0);
	jack_set_sample_rate_callback(jackClient, Synthesizer::updateSamplerate,
		0);
	jack_set_buffer_size_callback(jackClient, Synthesizer::updateBufferLength,
		0);

	// JACK sample data is always 32 bit float
	jackOutputPort = jack_port_register (jackClient, "out",
			"32 bit float mono audio", JackPortIsOutput, 0);
	if (jackOutputPort == NULL) {
		std::cout << "Problem in starting the synth: cannot register "
			"an output port in JACK." << std::endl;
	}

	samplerate = jack_get_sample_rate(jackClient);
	bufferLength = jack_get_buffer_size(jackClient);

	if (jack_activate (jackClient) != 0) {
		std::cout << "Problem in starting the synth: cannot activate "
			"JACK client" << std::endl;
		return;
	}
	synthIsRunning = true;
}

int Synthesizer::jackCallback(jack_nframes_t nframes, void * arg)
{
	synthInstance->processEvents();
	synthInstance->checkJackExceptions();
	synthInstance->generateSound(nframes);
	return 0;
}

void Synthesizer::checkJackExceptions()
{
	pthread_mutex_lock(&jackCallbackLock);
	if (samplerateChanged) {
		Oscillator::setSamplerate(samplerate);
		samplerateChanged = false;

	}
	if (bufferLengthChanged) {
		delete [] oscillatorBuffer;
		delete [] lfoBuffer;
		oscillatorBuffer = new float[bufferLength];
		lfoBuffer = new float[bufferLength];
		if (oscillatorBuffer == NULL || lfoBuffer == NULL) {
			std::cout <<  "Problem: buffer length changed, not enough "
				"free memory." << std::endl;
			synthIsRunning = false;
			pthread_mutex_unlock(&jackCallbackLock);
			jack_client_close(jackClient);
		}

		Oscillator::setBufferLength(bufferLength);
		bufferLengthChanged = false;
	}
	pthread_mutex_unlock(&jackCallbackLock);
}

void Synthesizer::generateSound(jack_nframes_t nframes)
{
	float * outputBuffer =
		(float *) jack_port_get_buffer(jackOutputPort, nframes);

	std::memset(outputBuffer, 0, nframes * sizeof(float));

	// Oscillator group 1
	bool useGlobalLfo = false;
	bool useDedicatedLfo = false;
	if (osc1parameters.lfoModulationTarget == FREQUENCY ||
		osc1parameters.lfoModulationTarget == PULSE_WIDTH)
	{
		useGlobalLfo = true;
	}
	if (osc1parameters.lfoModulationTarget == AMPLITUDE) {
		if (osc1parameters.lfoFrequencyType == RELATIVE) {
			useDedicatedLfo = true;
		}
		else {
			useGlobalLfo = true;
		}
	}

	if (useGlobalLfo) {
		// TODO
	}

	// TODO: remove this line when ready for LFO testing
	osc1parameters.lfoModulationTarget = NONE;

	unsigned int oscNum, i;
	bool noteFinished;
	for (oscNum = 0; oscNum < POLYPHONY; oscNum++) {
		if (oscillator1[oscNum]->getEnvelopePhase() != OFF) {
			if (useDedicatedLfo) {
				// TODO
			}
			oscillator1[oscNum]->generateSound(oscillatorBuffer, lfoBuffer,
				noteFinished);
			for (i = 0; i < nframes; i++) {
				outputBuffer[i] += oscillatorBuffer[i];
			}
		}
	}

	float mixingCoefficent = mainVolume / POLYPHONY;
	for (i = 0; i < nframes; i++) {
		outputBuffer[i] *= mixingCoefficent;
	}
}

int Synthesizer::updateSamplerate(jack_nframes_t nframes, void *arg)
{
	if (nframes != synthInstance->samplerate) {
		pthread_mutex_lock(&(synthInstance->jackCallbackLock));
		synthInstance->samplerate = nframes;
		synthInstance->samplerateChanged = true;
		pthread_mutex_unlock(&(synthInstance->jackCallbackLock));
	}
	return 0;
}

int Synthesizer::updateBufferLength(jack_nframes_t nframes, void *arg)
{
	if (nframes != synthInstance->bufferLength) {
		pthread_mutex_lock(&(synthInstance->jackCallbackLock));
		synthInstance->bufferLength = nframes;
		synthInstance->bufferLengthChanged = true;
		pthread_mutex_unlock(&(synthInstance->jackCallbackLock));
	}
	return 0;
}

void Synthesizer::processEvents()
{
	unsigned int dataLength = 0;
	unsigned char * ptr = events.swapBuffers(&dataLength);
	unsigned char * endPtr = ptr + dataLength;

	unsigned char key, velocity, parameterType;
	unsigned int parameterValue;
	NoteSource source;

	while (ptr < endPtr) {
		switch (*ptr++) {

		case 0x01: /* note on */
		key = *ptr++;
		velocity = *ptr++;
		if (*ptr++ == JACK) {
			source = JACK;
		}
		else {
			source = computerKeyboard;
		}
		processNoteOn(key, velocity, source);
		break;

		case 0x02: /* note off */
		key = *ptr++;
		if (*ptr++ == JACK) {
			source = JACK;
		}
		else {
			source = computerKeyboard;
		}
		processNoteOff(key, source);
		break;

		case 0x03: /* all notes off */
		if (*ptr++ == JACK) {
			source = JACK;
		}
		else {
			source = computerKeyboard;
		}
		processFastMute(source);
		break;

		case 0x04: /* parameter change */
		parameterType = *ptr++;
		parameterValue = (*ptr++) << 8;
		parameterValue += *ptr++;
		processParameterChange(parameterType, parameterValue);
		break;
		}
	}

}

void Synthesizer::processNoteOn(unsigned char key, unsigned char velocity,
NoteSource source)
{
	// First try to find and retrigger an oscillator group with the same
	// note key and note source
	unsigned int i;
	for (i = 0; i < POLYPHONY; i++) {
		if (noteKey[i] == key && noteSource[i] == source &&
			oscillator1[i]->getEnvelopePhase() != OFF)
		{
			oscillator1[i]->noteOn(key, velocity);
			return;
		}
	}

	// Then try to find a free oscillator group
	for (i = 0; i < POLYPHONY; i++) {
		if (oscillator1[i]->getEnvelopePhase() == OFF) {
			oscillator1[i]->noteOn(key, velocity);
			noteKey[i] = key;
			noteSource[i] = source;
			return;
		}
	}
}

void Synthesizer::processNoteOff(unsigned char key, NoteSource source)
{
	for (unsigned int i = 0; i < POLYPHONY; i++) {
		if (noteKey[i] == key && noteSource[i] == source) {
			oscillator1[i]->noteOff();
		}
	}
}

void Synthesizer::processFastMute(NoteSource source)
{
	for (unsigned int i = 0; i < POLYPHONY; i++) {
		if (noteSource[i] == source) {
			oscillator1[i]->muteFast();
		}
	}
}

void Synthesizer::processParameterChange(unsigned int parameter,
	unsigned int parameterValue)
{
	std::cout << "Synthesizer.processParameterChange(" << parameter << "," <<
		parameterValue << ")" << std::endl;

	if (parameter < 5) {
		switch (parameter) {

		case OSC1_WAVEFORM: // 0
			osc1parameters.waveform = (WaveformType)parameterValue;
			break;

		case OSC1_ATTACK:   // 1
			osc1parameters.attackTime = parameterValue;
			break;

		case OSC1_DECAY:    // 2
			osc1parameters.decayTime = parameterValue;
			break;

		case OSC1_SUSTAIN:  // 3
			osc1parameters.sustainVolume = parameterValue * 0.01;
			break;

		case OSC1_RELEASE:  // 4
			osc1parameters.releaseTime = parameterValue;
			break;
		}
	}
	else {
		switch (parameter) {
		case LFO1_FREQUENCY_TYPE:     // 5
			osc1parameters.lfoFrequencyType = (LfoFrequencyType)parameterValue;
			break;

		case LFO1_FIXED_FREQUENCY:    // 6
			osc1parameters.lfoFixedFrequency = parameterValue * 0.01;
			break;

		case LFO1_RELATIVE_FREQUENCY: // 7
			osc1parameters.lfoRelativeFrequency = parameterValue * 0.01;
			break;

		case LFO1_TARGET_TYPE:        // 8
			osc1parameters.lfoModulationTarget =
				(LfoModulationTarget) parameterValue;
			break;

		case LFO1_MODULATION_AMOUNT:  // 9
			osc1parameters.lfoModulationAmount = parameterValue * 0.01;
			break;

		case FILTER_LOWPASS:          // 20
			filter.setFrequency(parameterValue);
			break;
		}
	}
}
