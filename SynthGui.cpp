/*
 * SynthGui.cpp
 *
 *  Created on: 15.2.2012
 *      Author: alkim
 */

#include "SynthGui.h"

SynthGui * SynthGui::guiInstance;

SynthGui::SynthGui(EventBuffer & eventBuffer,
	SynthParameters & synthParameters) :
	synthEvents(eventBuffer), parameters(synthParameters)
{

	keyboardMutex = PTHREAD_MUTEX_INITIALIZER;
	guiInstance = this;

	generateKeyTranslations();

	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect(mainWindow, "focus_out_event", G_CALLBACK(
			SynthGui::guiFocusOutCallback), NULL);

	gtk_window_set_title(GTK_WINDOW(mainWindow), "maucisynth");
	gtk_widget_set_size_request(mainWindow, 1000, 300);

	/* Capture keyboard events */
	for (int i = 0; i < NUM_KEYS; i++) {
		keyIsPressed[i] = false;
	}
	baseOctave = 4;

	g_signal_connect(mainWindow, "key_press_event", G_CALLBACK(
			SynthGui::onKeyPressCallback), NULL);

	g_signal_connect(mainWindow, "key_release_event", G_CALLBACK(
			SynthGui::onKeyReleaseCallback), NULL);

	/* Contents of the main window */
	mainFixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(mainWindow), mainFixed); // @suppress("Function cannot be resolved")

	createFilterControls();
	createLfo1Controls();
	createOscillator1Controls();

	gtk_widget_show_all(mainWindow);
}

SynthGui::~SynthGui() {
}

void SynthGui::createFilterControls() {
    GtkWidget * frame;
    GtkWidget * fixed;
    GtkWidget * label;
    GtkWidget * vscale;
    
    frame = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(mainFixed), frame, 900, 0);
	gtk_frame_set_label(GTK_FRAME(frame), "Filtering");
	gtk_widget_set_size_request(GTK_WIDGET(frame), 100, 175);

	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(frame), fixed);

    label = gtk_label_new("Lowpass");
	gtk_fixed_put(GTK_FIXED(fixed), label, 5, 0);

	vscale = gtk_vscale_new_with_range(MinLowpassFrequency,
			MaxLowpassFrequency, 50);
	gtk_widget_set_size_request(GTK_WIDGET(vscale), 40, 128);
	gtk_range_set_inverted(GTK_RANGE(vscale), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(vscale), GTK_UPDATE_CONTINUOUS);
	gtk_range_set_value(GTK_RANGE(vscale),
			(MinLowpassFrequency + MaxLowpassFrequency) / 2);

	g_signal_connect(vscale, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"lowpassFilterFrequency");
	gtk_fixed_put(GTK_FIXED(fixed), vscale, 10, 20);
}

void SynthGui::createLfo1Controls() {
	GtkWidget * frame;
	GtkWidget * fixed;
	GtkWidget * button;
	GSList * group;
	GtkWidget * label;
	GtkWidget * scale;

	// Frame: LFO 1
	frame = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(mainFixed), frame, 0, 0);
	gtk_frame_set_label(GTK_FRAME(frame), "LFO 1");
	gtk_widget_set_size_request(GTK_WIDGET(frame), 488, 175);
	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(frame), fixed);

	label = gtk_label_new("Frequency");
	gtk_fixed_put(GTK_FIXED(fixed), label, 75, 0);


	// LFO 1 fixed frequency selector button
	button = gtk_radio_button_new_with_label(NULL, "Fixed\n(Hz)");
	g_signal_connect(button, "clicked",
		G_CALLBACK(SynthGui::buttonSelectCallback),
		(gpointer)"lfo1fixed");
	gtk_fixed_put(GTK_FIXED(fixed), button, 0, 19);

	// LFO 1 relative frequency selector button
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
	button = gtk_radio_button_new_with_label(group, "Relative\nto osc 1");
	g_signal_connect(button, "clicked",
		G_CALLBACK(SynthGui::buttonSelectCallback),
		(gpointer)"lfo1relative");
	gtk_fixed_put(GTK_FIXED(fixed), button, 0, 75);

	// LFO 1 fixed frequency slider
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
	scale = gtk_hscale_new_with_range(MinFixedLfoFrequency,
			MaxFixedLfoFrequency, 0.01);
	gtk_widget_set_size_request(GTK_WIDGET(scale), 128, 38);
	gtk_range_set_update_policy(GTK_RANGE(scale),
		GTK_UPDATE_CONTINUOUS);
	g_signal_connect(scale, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"lfo1fixed");
	gtk_fixed_put(GTK_FIXED(fixed), scale, 89, 22);

	// LFO 1 relative frequency slider
	scale = gtk_hscale_new_with_range(MinRelativeLfoFrequency,
			MaxRelativeLfoFrequency, 0.05);
	gtk_widget_set_size_request(GTK_WIDGET(scale), 128, 38);
	gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
	g_signal_connect(scale, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"lfo1relative");
	gtk_fixed_put(GTK_FIXED(fixed), scale, 89, 78);

	// LFO 1 modulation target: radio button group
	label = gtk_label_new("Target");
	gtk_fixed_put(GTK_FIXED(fixed), label, 310, 0);

	// LFO 1 modulation target: None
	button = gtk_radio_button_new_with_label(NULL, "None");
	g_signal_connect(button, "clicked",
		G_CALLBACK(SynthGui::buttonSelectCallback),
		(gpointer)"lfo1targetNone");
	gtk_fixed_put(GTK_FIXED(fixed), button, 250, 19);
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));

	// LFO 1 modulation target: frequency
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
	button = gtk_radio_button_new_with_label(group, "Osc 1 frequency");
	g_signal_connect(button, "clicked",
		G_CALLBACK(SynthGui::buttonSelectCallback),
		(gpointer)"lfo1targetFrequency");
	gtk_fixed_put(GTK_FIXED(fixed), button, 250, 44);

	// LFO 1 modulation target: amplitude
	button = gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(button), "Osc 1 amplitude");
	g_signal_connect(button, "clicked",
		G_CALLBACK(SynthGui::buttonSelectCallback),
		(gpointer)"lfo1targetAmplitude");
	gtk_fixed_put(GTK_FIXED(fixed), button, 250, 69);

	// LFO 1 modulation target: pulse width
	button = gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(button), "Osc 1 pulse width");
	g_signal_connect(button, "clicked",
		G_CALLBACK(SynthGui::buttonSelectCallback),
		(gpointer)"lfo1targetPulsewidth");
	gtk_fixed_put(GTK_FIXED(fixed), button, 250, 94);

	// LFO 1 modulation amount
	label = gtk_label_new("Amount");
	gtk_fixed_put(GTK_FIXED(fixed), label, 420, 0);
	scale = gtk_vscale_new_with_range(MinLfoModulation, MaxLfoModulation, 0.01);
	gtk_widget_set_size_request(GTK_WIDGET(scale), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(scale), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
	g_signal_connect(scale, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"lfo1modulationAmount");
	gtk_fixed_put(GTK_FIXED(fixed), scale, 430, 20);
}

void SynthGui::createOscillator1Controls() {
	GtkWidget * frame;
	GtkWidget * fixed;
	GSList * group;
	GtkWidget * label;
	GtkWidget * button;
	GtkWidget * triangleButton;
	GtkWidget * slider;

	/* Frame: Oscillator 1 */
	frame = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(mainFixed), frame, 500, 0);
	gtk_frame_set_label(GTK_FRAME(frame), "Oscillator 1");
	gtk_widget_set_size_request(GTK_WIDGET(frame), 225, 175);

	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(frame), fixed);

	/* Oscillator 1: waveform type */
	button = gtk_radio_button_new_with_label(NULL, "Sine");
	g_signal_connect(button, "clicked",
		G_CALLBACK(buttonSelectCallback),
		(gpointer)("osc1sine"));
	gtk_fixed_put(GTK_FIXED(fixed), button, 0, 0);

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
	triangleButton = gtk_radio_button_new_with_label(group, "Triangle");
	g_signal_connect(triangleButton, "clicked",
		G_CALLBACK(buttonSelectCallback),
		(gpointer)("osc1triangle"));
	gtk_fixed_put(GTK_FIXED(fixed), triangleButton, 0, 25);

	button = gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(triangleButton), "Sawtooth");
	g_signal_connect(button, "clicked",
		G_CALLBACK(buttonSelectCallback),
		(gpointer)("osc1sawtooth"));
	gtk_fixed_put(GTK_FIXED(fixed), button, 0, 50);

	button = gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(triangleButton), "Pulse");
	g_signal_connect(button, "clicked",
		G_CALLBACK(buttonSelectCallback),
		(gpointer)("osc1pulse"));
	gtk_fixed_put(GTK_FIXED(fixed), button, 0, 75);

	button = gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(triangleButton), "Abs(Sine)");
	g_signal_connect(button, "clicked",
		G_CALLBACK(buttonSelectCallback),
		(gpointer)("osc1abssine"));
	gtk_fixed_put(GTK_FIXED(fixed), button, 0, 100);

	/* Oscillator 1 : ADSR.
	 * Units of A, D and R are millisecods.
	 * Unit of R is percents of maximum attack amplitude. */
	label = gtk_label_new("A");
	gtk_fixed_put(GTK_FIXED(fixed), label, 100, 0);
	label = gtk_label_new("D");
	gtk_fixed_put(GTK_FIXED(fixed), label, 132, 0);
	label = gtk_label_new("S");
	gtk_fixed_put(GTK_FIXED(fixed), label, 164, 0);
	label = gtk_label_new("R");
	gtk_fixed_put(GTK_FIXED(fixed), label, 196, 0);

	slider = gtk_vscale_new_with_range(MinAttackTime, MaxAttackTime, 1);
	gtk_widget_set_size_request(GTK_WIDGET(slider), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(slider), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(slider), GTK_UPDATE_DELAYED);
	g_signal_connect(slider, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"osc1attack");
	gtk_fixed_put(GTK_FIXED(fixed), slider, 90, 20);

	slider = gtk_vscale_new_with_range(MinDecayTime, MaxDecayTime, 1);
	gtk_widget_set_size_request(GTK_WIDGET(slider), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(slider), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(slider), GTK_UPDATE_DELAYED);
	g_signal_connect(slider, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"osc1decay");
	gtk_fixed_put(GTK_FIXED(fixed), slider, 122, 20);

	slider = gtk_vscale_new_with_range(1, 100, 1);
	gtk_widget_set_size_request(GTK_WIDGET(slider), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(slider), TRUE);
	gtk_range_set_value(GTK_RANGE(slider), 100);
	gtk_range_set_update_policy(GTK_RANGE(slider), GTK_UPDATE_DELAYED);
	g_signal_connect(slider, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"osc1sustain");
	gtk_fixed_put(GTK_FIXED(fixed), slider, 154, 20);

	slider = gtk_vscale_new_with_range(MinReleaseTime, MaxReleaseTime, 1);
	gtk_widget_set_size_request(GTK_WIDGET(slider), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(slider), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(slider), GTK_UPDATE_DELAYED);
	g_signal_connect(slider, "value_changed",
		G_CALLBACK(SynthGui::sliderChangeCallback),
		(gpointer)"osc1release");
	gtk_fixed_put(GTK_FIXED(fixed), slider, 186, 20);
}

gboolean SynthGui::guiFocusOutCallback() {
	SynthGui::guiInstance->guiFocusOut();
	return FALSE;
}

gboolean SynthGui::onKeyPressCallback(GtkWidget * window, GdkEventKey * pKey,
		gpointer userdata) {
	SynthGui::guiInstance->onKeyPress(pKey);
	return FALSE;
}

gboolean SynthGui::onKeyReleaseCallback(GtkWidget * window, GdkEventKey * pKey,
		gpointer userdata) {
	SynthGui::guiInstance->onKeyRelease(pKey);
	return FALSE;
}

gboolean SynthGui::sliderChangeCallback(GtkAdjustment * adj, gpointer data) {
	SynthGui::guiInstance->sliderChange(adj, data);
	return FALSE;
}

gboolean SynthGui::buttonSelectCallback(GtkWidget * widget, gpointer data) {
	SynthGui::guiInstance->buttonSelect(widget, data);
	return FALSE;
}

void SynthGui::guiFocusOut() {
	int i;
	pthread_mutex_lock(&keyboardMutex);
	for (i = 0; i < NUM_KEYS; i++) {
		keyIsPressed[i] = false;
	}
	pthread_mutex_unlock(&keyboardMutex);
	synthEvents.addAllNotesOff(TEXT_KEYBOARD);
}

void SynthGui::onKeyPress(GdkEventKey * pKey) {
	int i, key, midiKey;
	if (pKey->type != GDK_KEY_PRESS) {
		return;
	}

	key = keyvalToIndex(pKey->keyval);
	if (key == NOT_INTERESTING_KEY) {
		return;
	}

	pthread_mutex_lock(&keyboardMutex);
	if (keyIsPressed[key] == TRUE) {
		pthread_mutex_unlock(&keyboardMutex);
		return;
	}

	if (key == OCTAVE_UP && baseOctave < BASE_OCTAVE_MAX) {
		for (i = 0; i < NUM_PIANO_KEYS; i++) {
			if (keyIsPressed[i] == true) {
				keyIsPressed[i] = false;
				midiKey = baseOctave * 12 + i;
				synthEvents.addNoteOff(midiKey, TEXT_KEYBOARD);
			}
		}
		baseOctave++;
	}
	if (key == OCTAVE_DOWN && baseOctave > BASE_OCTAVE_MIN) {
		for (i = 0; i < NUM_PIANO_KEYS; i++) {
			if (keyIsPressed[i] == true) {
				keyIsPressed[i] = false;
				midiKey = baseOctave * 12 + i;
				synthEvents.addNoteOff(midiKey, TEXT_KEYBOARD);
			}
		}
		baseOctave--;
	}
	if (key < NUM_PIANO_KEYS) {
		midiKey = baseOctave * 12 + key;
		synthEvents.addNoteOn(midiKey, 100, TEXT_KEYBOARD);
	}
	keyIsPressed[key] = true;
	pthread_mutex_unlock(&keyboardMutex);
}

void SynthGui::onKeyRelease(GdkEventKey * pKey) {
	int key, midiPitch;
	if (pKey->type != GDK_KEY_RELEASE) {
		return;
	}

	key = keyvalToIndex(pKey->keyval);
	if (key == NOT_INTERESTING_KEY) {
		return;
	}

	pthread_mutex_lock(&keyboardMutex);
	if (key < 25) {
		midiPitch = baseOctave * 12 + key;
		synthEvents.addNoteOff(midiPitch, TEXT_KEYBOARD);
	}
	keyIsPressed[key] = FALSE;
	pthread_mutex_unlock(&keyboardMutex);
}

void SynthGui::sliderChange(GtkAdjustment * adj, gpointer data) {
	float value = ((GtkRange *) adj)->adjustment->value;
	char * name = (char *)data;

	// Message strings:
	// lfo1fixed
	// lfo1relative
	// lfo1modulationAmount
	// lowpassFilterFrequency
	if (name[0] == 'l') {
		if (name[3] == '1') {
			if (name[4] == 'f') {
				parameters.osc1.lfoFixedFrequency = value;
				return;
			}
			if (name[4] == 'r') {
				parameters.osc1.lfoRelativeFrequency = value;
				return;
			}
			if (name[4] == 'm') {
				parameters.osc1.lfoModulationAmount = value;
				return;
			}
		}
		if (name[1] == 'o') {
			parameters.lowpassFrequency = value;
			return;
		}
	}

	// Message strings:
	// osc1attack
	// osc1decay
	// osc1sustain
	// osc1release
	if (name[0] == 'o') {
		if (name[3] == '1') {
			if (name[4] == 'a') {
				parameters.osc1.attackTime = value;
				return;
			}
			if (name[4] == 'd') {
				parameters.osc1.decayTime = value;
				return;
			}
			if (name[4] == 's') {
				parameters.osc1.sustainVolume = 0.01 * value;
				return;
			}
			if (name[4] == 'r') {
				parameters.osc1.releaseTime = value;
				return;
			}
		}
	}
}

void SynthGui::buttonSelect(GtkWidget * widget, gpointer data) {

	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		return;
	}
	char * name = (char *) data;

	// Message strings:
	// lfo1fixed
	// lfo1relative
	// lfo1targetAmplitude
	// lfo1targetFrequency
	// lfo1targetNone
	// lfo1targetPulsewidth
	if (name[0] == 'l') {
		if (name[3] == '1') {
			if (name[4] == 'f') {
				parameters.osc1.lfoFrequencyType = FIXED;
				return;
			}
			if (name[4] == 'r') {
				parameters.osc1.lfoFrequencyType = RELATIVE;
				return;
			}
			if (name[4] == 't') {
				if (name[10] == 'A') {
					parameters.osc1.lfoModulationTarget = AMPLITUDE;
					return;
				}
				if (name[10] == 'F') {
					parameters.osc1.lfoModulationTarget = FREQUENCY;
					return;
				}
				if (name[10] == 'N') {
					parameters.osc1.lfoModulationTarget = NONE;
					return;
				}
				if (name[10] == 'P') {
					parameters.osc1.lfoModulationTarget = PULSE_WIDTH;
					return;
				}
			}
		}
	}

	// Message strings:
	// osc1abssine
	// osc1pulse
	// osc1sawtooth
	// osc1sine
	// osc1triangle
	if (name[0] == 'o') {
		if (name[3] == '1') {
			if (name[4] == 'a') {
				parameters.osc1.waveform = ABS_SINE;
				return;
			}
			if (name[4] == 'p') {
				parameters.osc1.waveform = PULSE;
				return;
			}
			if (name[4] == 's') {
				if (name[5] == 'a') {
					parameters.osc1.waveform = SAWTOOTH;
					return;
				}
				if (name[5] == 'i') {
					parameters.osc1.waveform = SINE;
					return;
				}
			}
			if (name[4] == 't') {
				parameters.osc1.waveform = TRIANGLE;
				return;
			}
		}
	}
}

// Generates SynthGui.keyTranslations[] for method keyvalToIndex()
void SynthGui::generateKeyTranslations()
{
	int i;
	for (i = 0; i < 128; i++) {
		keyTranslations[i] = NOT_INTERESTING_KEY;
	}

	keyTranslations['Z'] = 0;
	keyTranslations['S'] = 1;
	keyTranslations['X'] = 2;
	keyTranslations['D'] = 3;
	keyTranslations['C'] = 4;
	keyTranslations['V'] = 5;
	keyTranslations['G'] = 6;
	keyTranslations['B'] = 7;
	keyTranslations['H'] = 8;
	keyTranslations['N'] = 9;
	keyTranslations['J'] = 10;
	keyTranslations['M'] = 11;
	keyTranslations[','] = 12;

	keyTranslations['Q'] = 12;
	keyTranslations['2'] = 13;
	keyTranslations['W'] = 14;
	keyTranslations['3'] = 15;
	keyTranslations['E'] = 16;
	keyTranslations['R'] = 17;
	keyTranslations['5'] = 18;
	keyTranslations['T'] = 19;
	keyTranslations['6'] = 20;
	keyTranslations['Y'] = 21;
	keyTranslations['7'] = 22;
	keyTranslations['U'] = 23;
	keyTranslations['I'] = 24;

	keyTranslations['z'] = 0;
	keyTranslations['s'] = 1;
	keyTranslations['x'] = 2;
	keyTranslations['d'] = 3;
	keyTranslations['c'] = 4;
	keyTranslations['v'] = 5;
	keyTranslations['g'] = 6;
	keyTranslations['b'] = 7;
	keyTranslations['h'] = 8;
	keyTranslations['n'] = 9;
	keyTranslations['j'] = 10;
	keyTranslations['m'] = 11;
	keyTranslations[','] = 12;

	keyTranslations['q'] = 12;
	keyTranslations['w'] = 14;
	keyTranslations['e'] = 16;
	keyTranslations['r'] = 17;
	keyTranslations['t'] = 19;
	keyTranslations['y'] = 21;
	keyTranslations['u'] = 23;
	keyTranslations['i'] = 24;

	keyTranslations['-'] = OCTAVE_DOWN;
	keyTranslations['+'] = OCTAVE_UP;
}

// Translates computer keyboard key value to relative note key value
int SynthGui::keyvalToIndex(guint keyval) {
	// Most of the key values are ASCII values

	if (keyval > 127)
		return NOT_INTERESTING_KEY;

	return keyTranslations[keyval];
}
