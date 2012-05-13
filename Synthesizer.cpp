/*
 * Synthesizer.cpp
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#include "Synthesizer.h"

Synthesizer * Synthesizer::synthInstance = NULL;

//
Synthesizer::Synthesizer(EventBuffer & b, SynthParameters & p):
	events(b), parameters(p), filter(p.lowpassFrequency)
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

	initJack();
	Oscillator::setSamplerate(samplerate);
	Oscillator::setBufferLength(bufferLength);
	filter.setSamplerate(samplerate);

	oscillatorBuffer = new float[bufferLength];
	lfoBuffer = new float[bufferLength];

	for (i = 0; i < POLYPHONY; i++) {
		oscillator1[i] = new MainOscillator(parameters.osc1);
		lfo1[i] = new LowFrequencyOscillator(parameters.osc1,
		                                     *(oscillator1[i]) );
	}

	if (lfo1[POLYPHONY - 1] == NULL) {
		std::cout << "Problem in starting the synth: not enough free memory."
			<< std::endl;
	}

	// Now everything is ready for sound rendering.
	// The following code requests JACK to start calling
	// Synthesizer::jackCallback().
	if (jack_activate (jackClient) != 0) {
		std::cout << "Problem in starting the synth: cannot activate "
			"JACK client" << std::endl;
		return;
	}
	synthIsRunning = true;
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

// Creates audio output and MIDI input ports into JACK
void Synthesizer::initJack()
{
	jack_options_t options = JackNoStartServer;
	jack_status_t status;
	jackClient = jack_client_open("maucisynth", options, &status);
	if (jackClient == NULL) {
		std::cout << "Problem in starting the synth. ";
		if (status & JackServerFailed) {
			std::cout << "Cannot connect to the JACK server. Maybe the JACK "
			"server program (jackd or QJackCtl) is not running.";
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

	// JACK sample data is always 32 bit float.
	// The string constant "32 bit float mono audio" is the same as
	// constant JACK_DEFAULT_AUDIO_TYPE in <jack/types.h>.
	jackOutputPort = jack_port_register (jackClient, "out",
			"32 bit float mono audio", JackPortIsOutput | JackPortIsTerminal,
			0);
	if (jackOutputPort == NULL) {
		std::cout << "Problem in starting the synth: cannot register "
			"the output port in JACK." << std::endl;
	}

	jackMidiInPort = jack_port_register(jackClient, "midi_in",
			JACK_DEFAULT_MIDI_TYPE, JackPortIsInput | JackPortIsTerminal, 0);
	if (jackMidiInPort == NULL) {
		std::cout << "Problem in starting the synth: cannot register "
			"the MIDI in port in JACK." << std::endl;
	}

	samplerate = jack_get_sample_rate(jackClient);
	bufferLength = jack_get_buffer_size(jackClient);
	midiInChannel = 0;
}

// Function that JACK calls when more sound data is needed.
// Parameter nframes is the JACK sound buffer length. It should be equal to
// synthInstance->bufferLength. Buffer length is typically 16..4096 samples.
// Parameter arg is not used.
int Synthesizer::jackCallback(jack_nframes_t nframes, void * arg)
{
	synthInstance->processGuiEvents();
	synthInstance->processMidiEvents(nframes);
	synthInstance->checkJackExceptions();
	synthInstance->generateSound(nframes);
	return 0;
}

// Handles samplerate and JACK audio buffer length changes.
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

// Synthesizes sound of the current notes and mixes the sound of the notes.
// Parameter nframes is number of samples to render.
void Synthesizer::generateSound(jack_nframes_t nframes)
{
	float * outputBuffer =
		(float *) jack_port_get_buffer(jackOutputPort, nframes);

	std::memset(outputBuffer, 0, nframes * sizeof(float));

	// Oscillator group 1
	bool useGlobalLfo = false;
	bool useDedicatedLfo = false;
	if (parameters.osc1.lfoModulationTarget != NONE) {
		if (parameters.osc1.lfoFrequencyType == RELATIVE) {
			useDedicatedLfo = true;
		}
		else {
			useGlobalLfo = true;
		}
	}

	if (useGlobalLfo) {
		lfo1[0]->generateSound(lfoBuffer);
	}

	unsigned int oscNum, i;
	bool noteFinished;
	for (oscNum = 0; oscNum < POLYPHONY; oscNum++) {
		if (oscillator1[oscNum]->getEnvelopePhase() != OFF) {
			if (useDedicatedLfo) {
				lfo1[oscNum]->generateSound(lfoBuffer);
			}
			oscillator1[oscNum]->generateSound(oscillatorBuffer, lfoBuffer,
				noteFinished);
			for (i = 0; i < nframes; i++) {
				outputBuffer[i] += oscillatorBuffer[i];
			}
		}
	}

#ifdef SYNTH_TESTING
	printOscillatorPhases();
#endif // SYNTH_TESTING

	float mixingCoefficent = parameters.mainVolume / POLYPHONY;
	for (i = 0; i < nframes; i++) {
		outputBuffer[i] *= mixingCoefficent;
	}

	filter.doFiltering(outputBuffer, bufferLength);
}

// JACK callback: samplerate changes
// Parameter nframes = samples per second.
// Parameter arg is not used.
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

// JACK callback: buffer length changes
// Parameter nframes = buffer length in samples
// Parameter arg is not used.
int Synthesizer::updateBufferLength(jack_nframes_t nframes, void * arg)
{
	if (nframes != synthInstance->bufferLength) {
		pthread_mutex_lock(&(synthInstance->jackCallbackLock));
		synthInstance->bufferLength = nframes;
		synthInstance->bufferLengthChanged = true;
		pthread_mutex_unlock(&(synthInstance->jackCallbackLock));
	}
	return 0;
}

// Processes events sent by GUI through an EventBuffer.
void Synthesizer::processGuiEvents()
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
		if (*ptr++ == jackMidi) {
			source = jackMidi;
		}
		else {
			source = computerKeyboard;
		}
		processNoteOn(key, velocity, source);
		break;

		case 0x02: /* note off */
		key = *ptr++;
		if (*ptr++ == jackMidi) {
			source = jackMidi;
		}
		else {
			source = computerKeyboard;
		}
		processNoteOff(key, source);
		break;

		case 0x03: /* all notes off */
		if (*ptr++ == jackMidi) {
			source = jackMidi;
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

// Processes MIDI events from JACK.
void Synthesizer::processMidiEvents(jack_nframes_t nframes)
{
	void * midiBuffer = jack_port_get_buffer(jackMidiInPort, nframes);

	jack_nframes_t eventCount;
	eventCount = jack_midi_get_event_count(midiBuffer);

	jack_nframes_t eventNumber;
	jack_midi_event_t event;
	unsigned char * ptr;
	unsigned char * endPtr;
	unsigned char channel;
	for (eventNumber = 0; eventNumber < eventCount; eventNumber++) {

		jack_midi_event_get(&event, midiBuffer, eventNumber);

		ptr = event.buffer;
		channel = *ptr & 0x0F;
		if (channel != this->midiInChannel) {
			continue;
		}
		switch (*ptr & 0xF0) {
		case 0x80: // Note Off
			processNoteOff(ptr[1], jackMidi);
			break;

		case 0x90: // Note on
			processNoteOn(ptr[1], ptr[2], jackMidi);
			break;


		case 0xB0: // Controller change
			processMidiControlChange(ptr[1], ptr[2]);
			break;

		}

		// Print event data
		std::cout << "MIDI Event: time=" << event.time << ", data:";
		std::cout << std::hex;
		ptr = (unsigned char *) event.buffer;
		endPtr = ptr + event.size;
		while (ptr < endPtr) {
			std::cout << " " << (int)(*ptr);
			ptr++;
		}
		std::cout << std::endl << std::dec;
	}
	std::cout.flush();
}

// Processes MIDI "control change" event
void Synthesizer::processMidiControlChange(unsigned char type,
	unsigned char value)
{
	switch (type) {
	// case 0x07: // Channel volume (MSB)
	// case 0x27: // Channel volume (LSB)
	case 0x7b: // All notes off
		processFastMute(jackMidi);
	}
}

// Processes a "Note on" event from GUI or JACK MIDI port.
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
			oscillator1[i]->noteOn(key, velocity, true);
			return;
		}
	}

	// Then try to find a free oscillator group
	for (i = 0; i < POLYPHONY; i++) {
		if (oscillator1[i]->getEnvelopePhase() == OFF) {
			oscillator1[i]->noteOn(key, velocity, false);
			noteKey[i] = key;
			noteSource[i] = source;
			return;
		}
	}
}

// Processes a "Note off" event from GUI or JACK MIDI port.
void Synthesizer::processNoteOff(unsigned char key, NoteSource source)
{
	for (unsigned int i = 0; i < POLYPHONY; i++) {
		if (noteKey[i] == key && noteSource[i] == source &&
			oscillator1[i]->getEnvelopePhase() != OFF)
		{
			oscillator1[i]->noteOff();
		}
	}
}

// Processes an "All notes off" event from GUI or JACK MIDI port.
void Synthesizer::processFastMute(NoteSource source)
{
	for (unsigned int i = 0; i < POLYPHONY; i++) {
		if (noteSource[i] == source) {
			oscillator1[i]->muteFast();
		}
	}
}

// Processes a "Parameter change" event from GUI or JACK MIDI port.
void Synthesizer::processParameterChange(unsigned int parameter,
	unsigned int parameterValue)
{
#ifdef SYNTH_TESTING
	std::cout << "Synthesizer.processParameterChange(" << parameter << "," <<
		parameterValue << ")" << std::endl;
#endif // SYNTH_TESTING

	if (parameter < 5) {
		switch (parameter) {

		case OSC1_WAVEFORM: // 0
			parameters.osc1.waveform = (WaveformType)parameterValue;
			break;

		case OSC1_ATTACK:   // 1
			parameters.osc1.attackTime = parameterValue;
			break;

		case OSC1_DECAY:    // 2
			parameters.osc1.decayTime = parameterValue;
			break;

		case OSC1_SUSTAIN:  // 3
			parameters.osc1.sustainVolume = parameterValue * 0.01;
			break;

		case OSC1_RELEASE:  // 4
			parameters.osc1.releaseTime = parameterValue;
			break;
		}
	}
	else {
		switch (parameter) {
		case LFO1_FREQUENCY_TYPE:     // 5
			parameters.osc1.lfoFrequencyType = (LfoFrequencyType)parameterValue;
			break;

		case LFO1_FIXED_FREQUENCY:    // 6
			parameters.osc1.lfoFixedFrequency = parameterValue * 0.01;
			break;

		case LFO1_RELATIVE_FREQUENCY: // 7
			parameters.osc1.lfoRelativeFrequency = parameterValue * 0.01;
			break;

		case LFO1_TARGET_TYPE:        // 8
			parameters.osc1.lfoModulationTarget =
				(LfoModulationTarget) parameterValue;
			break;

		case LFO1_MODULATION_AMOUNT:  // 9
			parameters.osc1.lfoModulationAmount = parameterValue * 0.01;
			break;

		case FILTER_LOWPASS:          // 20
			parameters.lowpassFrequency =  parameterValue;
			break;
		}
	}
}

#ifdef SYNTH_TESTING
// Test method that prints the envelope curve phase of all oscillators
// when the phase of some oscillator changes.
void Synthesizer::printOscillatorPhases()
{
	unsigned int i;
	bool phasesChanged = false;
	for (i = 0; i < POLYPHONY; i++) {
		osc1curPhase[i] = oscillator1[i]->getEnvelopePhase();
		if (osc1curPhase[i] != osc1prevPhase[i]) {
			phasesChanged = true;
		}
	}
	if (phasesChanged) {
		std::cout << "generateSound:";
		for (i = 0; i < POLYPHONY; i++) {
			std::cout << "|";
			switch(osc1curPhase[i]) {
			case ATTACK: std::cout << "A"; break;
			case DECAY: std::cout << "D"; break;
			case SUSTAIN: std::cout << "S"; break;
			case RELEASE: std::cout << "R"; break;
			case OFF:     std::cout << " "; break;
			case FAST_MUTE: std::cout << "M"; break;
			case RETRIGGER: std::cout << "T"; break;
			}
			osc1prevPhase[i] = osc1curPhase[i];
		}
		std::cout << std::endl;
	}

}

#endif // SYNTH_TESTING
