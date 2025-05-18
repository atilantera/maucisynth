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

#define ENVELOPE_CHANGES_PER_BUFFER 4

class EnvelopeGenerator {
public:
	EnvelopeGenerator(int sampleRate, int bufferLength);

	void generateEnvelope(float * outputBuffer);
	void addEnvelopeChange(int time, EnvelopePhase phase);
    EnvelopePhase getPhase() const;

    void setAttackTime(int samples);
    void setDecayTime(int samples);
    void setSustainLevel(float level);
    void setReleaseTime(int samples);

private:
	// Sample rate in samples per second
	int sampleRate;

	// Buffer length in samples per buffer
	int bufferLength;

    // Maximum number of envelope changes per buffer
    int envelopeChangeCount;
    int envelopeChangeTime[ENVELOPE_CHANGES_PER_BUFFER];
    int envelopeChangePhase[ENVELOPE_CHANGES_PER_BUFFER];


	// Parameters: Amplitude envelope (ADSR curve) parameters.
	// Attack, decay and release are in samples.
	// Sustain is relative amplitude: 0..1.
	int attackTime;
	int decayTime;
	float sustainLevel;
	int releaseTime;

    // envelopePhase is the current ADSR curve phase.
   	EnvelopePhase envelopePhase;

    void renderAttack(float * outputBuffer);
    void renderDecay(float * outputBuffer);
    void renderSustain(float * outputBuffer);
    void renderRelease(float * outputBuffer);

};

#endif /* ENVELOPEGENERATOR_H_ */
