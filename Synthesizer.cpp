/*
 * Synthesizer.cpp
 *
 *  Created on: 11.3.2012
 *      Author: alkim
 */

#include "Synthesizer.h"

Synthesizer * Synthesizer::synthInstance = NULL;

Synthesizer::Synthesizer(EventBuffer & b): events(b)
{
	unsigned int i;

	synthInstance = this;
	samplerate = 1;
	bufferLength = 0;
	jackIsRunning = false;

	oscillatorBuffer = NULL;
	lfoBuffer = NULL;

	oscillatorBuffer = new float[bufferLength];
	lfoBuffer = new float[bufferLength];

	for (i = 0; i < POLYPHONY; i++) {
		oscillatorTable[i] = new MainOscillator();
		lfoTable[i] = new LowFrequencyOscillator();
	}

	initJack();
}

Synthesizer::~Synthesizer() {
	unsigned int i;

	jack_client_close(jackClient);
	delete [] oscillatorBuffer;
	delete [] lfoBuffer;

	for (i = 0; i < POLYPHONY; i++) {
		delete oscillatorTable[i];
		delete lfoTable[i];
	}
}

bool Synthesizer::isActive()
{
	return jackIsRunning;
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
	jackIsRunning = true;
}

int Synthesizer::jackCallback(jack_nframes_t nframes, void * arg)
{
	synthInstance->processEvents();
	synthInstance->generateSound(nframes);
	return 0;
}

inline void Synthesizer::generateSound(jack_nframes_t nframes)
{
	float * outputBuffer =
		(float *) jack_port_get_buffer(jackOutputPort, nframes);
}

int Synthesizer::updateSamplerate(jack_nframes_t nframes, void *arg)
{
	// TODO
	// samplerate = nframes;
	std::cout << "Synthesizer: sample rate changed to " << nframes
		<< std::endl;

	// Returns zero on success
	return 0;
}

int Synthesizer::updateBufferLength(jack_nframes_t nframes, void *arg)
{
	// TODO
	// bufferLength = nframes;
	std::cout << "Synthesizer: buffer length changed to " << nframes
		<< std::endl;

	// Returns zero on success
	return 0;
}

void Synthesizer::processEvents()
{
	unsigned int dataLength = 0;
	unsigned char * ptr = events.swapBuffers(&dataLength);
	unsigned char * endPtr = ptr + dataLength;

	unsigned int key, velocity, noteSource, parameterType, parameterValue;

	while (ptr < endPtr) {
		switch (*ptr) {

		case 0x01: /* note on */
		ptr++;
		key = *ptr++;
		velocity = *ptr++;
		noteSource = *ptr++;
		processNoteOn(key, velocity, noteSource);
		break;

		case 0x02: /* note off */
		ptr++;
		key = *ptr++;
		noteSource = *ptr++;
		processNoteOff(key, noteSource);
		break;

		case 0x03: /* all notes off */
		ptr++;
		noteSource = *ptr++;
		processFastMute(noteSource);
		break;

		case 0x04: /* parameter change */
		ptr++;
		parameterType = *ptr++;
		parameterValue = (*ptr++) << 8;
		parameterValue += *ptr++;
		processParameterChange(parameterType, parameterValue);
		break;
		}
	}

}

void Synthesizer::processNoteOn(unsigned char key, unsigned char velocity,
unsigned char noteSource)
{
	// TODO: see handle_messages in synth.c
}

void Synthesizer::processNoteOff(unsigned char key, unsigned char noteSource)
{
	// TODO: see handle_messages in synth.c
}

void Synthesizer::processFastMute(unsigned char noteSource)
{
	// TODO: see handle_messages in synth.c
}

void Synthesizer::processParameterChange(unsigned int parameter,
	unsigned int parameterValue)
{
	// TODO: apply handle_parameter_change from synth.c
}
