/*
 * Envelope.h
 *
 * ADSR envelope curve for oscillator amplitude and filter parameter
 * changes.
 */

#ifndef ENVELOPEGENERATOR_H_
#define ENVELOPEGENERATOR_H_

enum EnvelopePhase { ATTACK, DECAY, SUSTAIN, RELEASE, OFF, FAST_MUTE,
    RETRIGGER };

class EnvelopeGenerator {
public:
	EnvelopeGenerator(unsigned int sampleRate, unsigned int bufferLength);

	void generateEnvelope(float * outputBuffer);
	void addEnvelopeChange(unsigned int time, EnvelopePhase phase);
    EnvelopePhase getPhase() const;

private:
	// Sample rate in samples per second
	unsigned int sampleRate;

	// Buffer length in samples per buffer
	unsigned int bufferLength;

	// Parameters: Amplitude envelope (ADSR curve) parameters.
	// Attack, decay and release are in samples.
	// Sustain is relative amplitude: 0..1.
	unsigned int attackTime;
	unsigned int decayTime;
	float sustainVolume;
	unsigned int releaseTime;

    // envelopePhase is the current ADSR curve phase.
   	EnvelopePhase envelopePhase;

};

#endif /* ENVELOPEGENERATOR_H_ */
