/*
 * OscillatorControls.h
 *
 *  Created on: 16.2.2012
 *      Author: alkim
 */

#ifndef OSCILLATORCONTROLS_H_
#define OSCILLATORCONTROLS_H_

#include <gtk-2.0/gtk/gtk.h>

class OscillatorControls {
public:
	OscillatorControls(GtkWidget * mainFixed, int xPosition, int yPosition);
	~OscillatorControls();

private:
	// Widget creators
	void createLfoControls();
	void createOscControls();

	// These static members are wrappers for GTK+ callbacks.
	// C++ nonstatic member function pointers are incompatible
	// with C function pointers.
	// Call chain: GTK+ -> guiFocusOutCallback ->
	//     SynthGui->guiInstance->guiFocusOut

	// Actual GUI functionality
	void lfoButtonSelect(GtkWidget * widget, gpointer data);
	void oscWaveformSelect(GtkWidget * widget, gpointer data);

	// Member data
	GtkWidget * mainFixed_;
	int xPosition_;
	int yPosition_;
	GtkWidget * lfoFixedSelect;
	GtkWidget * lfoRelativeSelect;
	GtkWidget * lfoFixedFrequency;
	GtkWidget * lfoRelativeFrequency;
	GtkWidget * lfoTargetToNone;
	GtkWidget * lfoTargetToFrequency;
	GtkWidget * lfoTargetToAmplitude;
	GtkWidget * lfoTargetToPulseWidth;
	GtkWidget * lfoModulationAmount;
	GtkWidget * oscSineSelect;
	GtkWidget * oscTriangleSelect;
	GtkWidget * oscSawtoothSelect;
	GtkWidget * oscPulseSelect;
	GtkWidget * oscAbsSineSelect;
	GtkWidget * oscAttack;
	GtkWidget * oscDecay;
	GtkWidget * oscRelease;
	GtkWidget * oscSustain;
};

#endif /* OSCILLATORCONTROLS_H_ */
