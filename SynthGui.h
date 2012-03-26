/*
 * SynthGui.h
 * SynthGui contains all GUI functionality: window, widgets.
 * It also captures key presses and releases from the computer keyboard
 * when the GUI window is active.
 *
 *  Created on: 15.2.2012
 *      Author: alkim
 */

#ifndef SYNTHGUI_H_
#define SYNTHGUI_H_

#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <iostream>
#include <string>
#include <gtk-2.0/gdk/gdkkeysyms.h>
#include <gtk-2.0/gtk/gtk.h>
#include "EventBuffer.h"
#include "SynthParameters.h"

const int NUM_KEYS = 28;
const int NUM_PIANO_KEYS = 25;
const int OCTAVE_DOWN = 25;
const int OCTAVE_UP = 26;
const int NOT_INTERESTING_KEY = 1000;
const int BASE_OCTAVE_MAX = 8;
const int BASE_OCTAVE_MIN = 0;

class SynthGui {
public:
	SynthGui(EventBuffer & eventBuffer);
	~SynthGui();

	void endExecution();

private:
	// Widget creators
	void createLfo1Controls();
	void createOscillator1Controls();
	void createFilterControls();

	// These static members are wrappers for GTK+ callbacks.
	// C++ nonstatic member function pointers are incompatible
	// with C function pointers.
	// Call chain: GTK+ -> guiFocusOutCallback ->
	//     SynthGui->guiInstance->guiFocusOut
	static SynthGui * guiInstance;
	static gboolean guiFocusOutCallback();
	static gboolean onKeyPressCallback(GtkWidget * window, GdkEventKey * pKey,
			gpointer userdata);

	static gboolean onKeyReleaseCallback(GtkWidget * window,
			GdkEventKey * pKey, gpointer userdata);

	static gboolean sliderChangeCallback(GtkAdjustment * adj, gpointer data);
	static gboolean buttonSelectCallback(GtkWidget * widget, gpointer data);

	// Actual GUI functionality
	void guiFocusOut();
	void onKeyPress(GdkEventKey * pKey);
	void onKeyRelease(GdkEventKey * pKey);
	void sliderChange(GtkAdjustment * adj, gpointer data);
	void waveformSelect(GtkWidget * widget, gpointer data);
	void buttonSelect(GtkWidget * widget, gpointer data);

	void generateKeyTranslations();
	int keyvalToIndex(guint keyval);

	// Member data
	pthread_mutex_t keyboardMutex;
	bool keyIsPressed[NUM_KEYS];
	int baseOctave;
	EventBuffer & synthEvents;

	GtkWidget * mainWindow;
	GtkWidget * mainFixed;

	int keyTranslations[128];
	bool endExecutionRegistered;
};

#endif /* SYNTHGUI_H_ */
