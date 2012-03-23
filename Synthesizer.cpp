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
	synthInstance = this;
	samplerate = 1;
	bufferLength = 0;

	// Start JACK
	jackIsRunning = false;
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

	// JACK sample data is always 32 bit float
	jackOutputPort = jack_port_register (jackClient, "out",
			"32 bit float mono audio", JackPortIsOutput, 0);
	if (jackOutputPort == NULL) {
		std::cout << "Problem in starting the synth: cannot register "
			"an output port in JACK." << std::endl;
	}

	bufferLength = jack_get_buffer_size(jackClient);
	samplerate = jack_get_sample_rate(jackClient);

	if (jack_activate (jackClient) != 0) {
		std::cout << "Problem in starting the synth: cannot activate "
			"JACK client" << std::endl;
		return;
	}
	jackIsRunning = true;
}

Synthesizer::~Synthesizer() {
	jack_client_close(jackClient);
}

bool Synthesizer::isActive()
{
	return jackIsRunning;
}

int Synthesizer::jackCallback(jack_nframes_t nframes, void * arg)
{
	synthInstance->generateSound(nframes);
	return 0;
}

inline void Synthesizer::generateSound(jack_nframes_t nframes)
{
	float * outputBuffer =
		(float *) jack_port_get_buffer(jackOutputPort, nframes);
}
