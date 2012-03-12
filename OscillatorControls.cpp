/*
 * OscillatorControls.cpp
 *
 *  Created on: 16.2.2012
 *      Author: alkim
 */

#include "OscillatorControls.h"

OscillatorControls::OscillatorControls(GtkWidget * mainFixed, int xPosition,
		int yPosition) {
	mainFixed_ = mainFixed;
	xPosition_ = xPosition;
	yPosition_ = yPosition;

}

OscillatorControls::~OscillatorControls() {
	// TODO Auto-generated destructor stub
}

void OscillatorControls::createLfoControls(int oscNumber) {
	GtkWidget * frame;
	GtkWidget * fixed;
	GSList * group;
	GtkWidget * label;

	/* Frame: LFO*/
	frame = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(main_fixed), frame, xPosition_, yPosition_);
	gtk_frame_set_label(GTK_FRAME(frame), "LFO 1");
	gtk_widget_set_size_request(GTK_WIDGET(frame), 488, 175);
	gtk_widget_show(frame);

	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(frame), fixed);
	gtk_widget_show(fixed);

	/* Frequency parameters */
	label = gtk_label_new("Frequency");
	gtk_fixed_put(GTK_FIXED(fixed), label, 75, 0);
	gtk_widget_show(label);

	lfoFixedSelect = gtk_radio_button_new_with_label(NULL, "Fixed\n(Hz)");
	g_signal_connect(lfoFixedSelect, "clicked", G_CALLBACK(
			lfo1_button_select), (gpointer)("0"));
	gtk_fixed_put(GTK_FIXED(fixed), lfoFixedSelect, 0, 19);
	gtk_widget_show( lfoFixedSelect);

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(lfoFixedSelect));
	lfoRelativeSelect = gtk_radio_button_new_with_label(group,
			"Relative\n(of osc 1)");
	g_signal_connect(lfoRelativeSelect, "clicked", G_CALLBACK(
			lfo1_button_select), (gpointer)("1"));
	gtk_fixed_put(GTK_FIXED(fixed), lfoRelativeSelect, 0, 75);
	gtk_widget_show( lfoRelativeSelect);

	lfoFixedFrequency = gtk_hscale_new_with_range(0.1, 50, 0.1);
	gtk_widget_set_size_request(GTK_WIDGET(lfoFixedFrequency), 128, 38);
	gtk_range_set_update_policy(GTK_RANGE(lfoFixedFrequency),
			GTK_UPDATE_CONTINUOUS);
	g_signal_connect(lfoFixedFrequency, "value_changed", G_CALLBACK(
			slider_change), NULL);
	gtk_fixed_put(GTK_FIXED(fixed), lfoFixedFrequency, 89, 22);
	gtk_widget_show( lfoFixedFrequency);

	lfoRelativeFrequency = gtk_hscale_new_with_range(0.05, 0.5, 0.05);
	gtk_widget_set_size_request(GTK_WIDGET(lfoRelativeFrequency), 128, 38);
	gtk_range_set_update_policy(GTK_RANGE(lfoRelativeFrequency),
			GTK_UPDATE_CONTINUOUS);
	g_signal_connect(lfoRelativeFrequency, "value_changed", G_CALLBACK(
			slider_change), NULL);
	gtk_fixed_put(GTK_FIXED(fixed), lfoRelativeFrequency, 89, 78);
	gtk_widget_show( lfoRelativeFrequency);

	/* Modulation target */
	label = gtk_label_new("Target");
	gtk_fixed_put(GTK_FIXED(fixed), label, 310, 0);
	gtk_widget_show(label);

	lfoTargetToNone = gtk_radio_button_new_with_label(NULL, "None");
	g_signal_connect(lfoTargetToNone, "clicked",
			G_CALLBACK(lfo1_button_select), (gpointer)("2"));
	gtk_fixed_put(GTK_FIXED(fixed), lfoTargetToNone, 250, 19);
	gtk_widget_show( lfoTargetToNone);

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(lfoTargetToNone));
	lfoTargetToFrequency = gtk_radio_button_new_with_label(group,
			"Osc 1 frequency");
	g_signal_connect(lfoTargetToFrequency, "clicked", G_CALLBACK(
			lfo1_button_select), (gpointer)("3"));
	gtk_fixed_put(GTK_FIXED(fixed), lfoTargetToFrequency, 250, 44);
	gtk_widget_show( lfoTargetToFrequency);

	lfoTargetToAmplitude = gtk_radio_button_new_with_label_from_widget(
			GTK_RADIO_BUTTON(lfoTargetToFrequency), "Osc 1 amplitude");
	g_signal_connect(lfoTargetToAmplitude, "clicked", G_CALLBACK(
			lfo1_button_select), (gpointer)("4"));
	gtk_fixed_put(GTK_FIXED(fixed), lfoTargetToAmplitude, 250, 69);
	gtk_widget_show( lfoTargetToAmplitude);

	lfoTargetToPulseWidth = gtk_radio_button_new_with_label_from_widget(
			GTK_RADIO_BUTTON(lfoTargetToFrequency), "Osc 1 pulse width");
	g_signal_connect(lfoTargetToPulseWidth, "clicked", G_CALLBACK(
			lfo1_button_select), (gpointer)("5"));
	gtk_fixed_put(GTK_FIXED(fixed), lfoTargetToPulseWidth, 250, 94);
	gtk_widget_show( lfoTargetToPulseWidth);

	/* Modulation amount */
	label = gtk_label_new("Amount");
	gtk_fixed_put(GTK_FIXED(fixed), label, 420, 0);
	gtk_widget_show(label);

	lfoModulationAmount = gtk_vscale_new_with_range(0.01, 1, 0.01);
	gtk_widget_set_size_request(GTK_WIDGET(lfoModulationAmount), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(lfoModulationAmount), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(lfoModulationAmount),
			GTK_UPDATE_CONTINUOUS);
	g_signal_connect(lfoModulationAmount, "value_changed", G_CALLBACK(
			slider_change), NULL);
	gtk_fixed_put(GTK_FIXED(fixed), lfoModulationAmount, 430, 20);
	gtk_widget_show( lfoModulationAmount);
}

void OscillatorControls::create_osc1_controls(GtkWidget * main_fixed) {
	GtkWidget * frame;
	GtkWidget * fixed;
	GSList * group;
	GtkWidget * label;

	/* Frame: Oscillator 1 */
	frame = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(main_fixed), frame, 500, 0);
	gtk_frame_set_label(GTK_FRAME(frame), "Oscillator 1");
	gtk_widget_set_size_request(GTK_WIDGET(frame), 225, 175);
	gtk_widget_show(frame);

	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(frame), fixed);
	gtk_widget_show(fixed);

	/* Oscillator 1: waveform type */
	oscSineSelect = gtk_radio_button_new_with_label(NULL, "Sine");
	g_signal_connect(oscSineSelect, "clicked", G_CALLBACK(
			osc1_waveform_select), (gpointer)("0"));
	gtk_fixed_put(GTK_FIXED(fixed), oscSineSelect, 0, 0);
	gtk_widget_show( oscSineSelect);

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(oscSineSelect));
	oscTriangleSelect = gtk_radio_button_new_with_label(group, "Triangle");
	g_signal_connect(oscTriangleSelect, "clicked", G_CALLBACK(
			osc1_waveform_select), (gpointer)("1"));
	gtk_fixed_put(GTK_FIXED(fixed), oscTriangleSelect, 0, 25);
	gtk_widget_show( oscTriangleSelect);

	oscSawtoothSelect = gtk_radio_button_new_with_label_from_widget(
			GTK_RADIO_BUTTON(oscTriangleSelect), "Sawtooth");
	g_signal_connect(oscSawtoothSelect, "clicked", G_CALLBACK(
			osc1_waveform_select), (gpointer)("2"));
	gtk_fixed_put(GTK_FIXED(fixed), oscSawtoothSelect, 0, 50);
	gtk_widget_show( oscSawtoothSelect);

	oscPulseSelect = gtk_radio_button_new_with_label_from_widget(
			GTK_RADIO_BUTTON(oscTriangleSelect), "Pulse");
	g_signal_connect(oscPulseSelect, "clicked", G_CALLBACK(
			osc1_waveform_select), (gpointer)("3"));
	gtk_fixed_put(GTK_FIXED(fixed), oscPulseSelect, 0, 75);
	gtk_widget_show( oscPulseSelect);

	oscAbsSineSelect = gtk_radio_button_new_with_label_from_widget(
			GTK_RADIO_BUTTON(oscTriangleSelect), "Abs(Sine)");
	g_signal_connect(oscAbsSineSelect, "clicked", G_CALLBACK(
			osc1_waveform_select), (gpointer)("4"));
	gtk_fixed_put(GTK_FIXED(fixed), oscAbsSineSelect, 0, 100);
	gtk_widget_show( oscAbsSineSelect);

	/* Oscillator 1 : ADSR.
	 * Units of A, D and R are millisecods.
	 * Unit of R is percents of maximum attack amplitude. */
	label = gtk_label_new("A");
	gtk_fixed_put(GTK_FIXED(fixed), label, 100, 0);
	gtk_widget_show(label);
	oscAttack = gtk_vscale_new_with_range(1, 1000, 5);
	gtk_widget_set_size_request(GTK_WIDGET(oscAttack), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(oscAttack), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(oscAttack), GTK_UPDATE_CONTINUOUS);
	g_signal_connect(oscAttack, "value_changed", G_CALLBACK(slider_change),
			NULL);
	gtk_fixed_put(GTK_FIXED(fixed), oscAttack, 90, 20);
	gtk_widget_show( oscAttack);

	label = gtk_label_new("D");
	gtk_fixed_put(GTK_FIXED(fixed), label, 132, 0);
	gtk_widget_show(label);
	oscDecay = gtk_vscale_new_with_range(1, 1000, 5);
	gtk_widget_set_size_request(GTK_WIDGET(oscDecay), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(oscDecay), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(oscDecay), GTK_UPDATE_CONTINUOUS);
	g_signal_connect(oscDecay, "value_changed", G_CALLBACK(slider_change),
			NULL);
	gtk_fixed_put(GTK_FIXED(fixed), oscDecay, 122, 20);
	gtk_widget_show( oscDecay);

	label = gtk_label_new("S");
	gtk_fixed_put(GTK_FIXED(fixed), label, 164, 0);
	gtk_widget_show(label);
	oscSustain = gtk_vscale_new_with_range(1, 100, 1);
	gtk_widget_set_size_request(GTK_WIDGET(oscSustain), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(oscSustain), TRUE);
	gtk_range_set_value(GTK_RANGE(oscSustain), 100);
	gtk_range_set_update_policy(GTK_RANGE(oscSustain), GTK_UPDATE_CONTINUOUS);
	g_signal_connect(oscSustain, "value_changed", G_CALLBACK(slider_change),
			NULL);
	gtk_fixed_put(GTK_FIXED(fixed), oscSustain, 154, 20);
	gtk_widget_show( oscSustain);

	label = gtk_label_new("R");
	gtk_fixed_put(GTK_FIXED(fixed), label, 196, 0);
	gtk_widget_show(label);
	oscRelease = gtk_vscale_new_with_range(1, 3000, 30);
	gtk_widget_set_size_request(GTK_WIDGET(oscRelease), 32, 128);
	gtk_range_set_inverted(GTK_RANGE(oscRelease), TRUE);
	gtk_range_set_update_policy(GTK_RANGE(oscRelease), GTK_UPDATE_CONTINUOUS);
	g_signal_connect(oscRelease, "value_changed", G_CALLBACK(slider_change),
			NULL);
	gtk_fixed_put(GTK_FIXED(fixed), oscRelease, 186, 20);
	gtk_widget_show( oscRelease);

}

void OscillatorControls::osc1_waveform_select(GtkWidget * widget, gpointer data) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		unsigned char * str = (unsigned char *) data;
		switch (str[0]) {

		case '0':
			synth_parameter_change(OSC1_WAVEFORM, SINE);
			break;

		case '1':
			synth_parameter_change(OSC1_WAVEFORM, TRIANGLE);
			break;

		case '2':
			synth_parameter_change(OSC1_WAVEFORM, SAWTOOTH);
			break;

		case '3':
			synth_parameter_change(OSC1_WAVEFORM, PULSE);
			break;

		case '4':
			synth_parameter_change(OSC1_WAVEFORM, ABS_SINE);
			break;

		}
	}
}

void OscillatorControls::lfo1_button_select(GtkWidget * widget, gpointer data) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		unsigned char * str = (unsigned char *) data;
		switch (str[0]) {

		case '0':
			synth_parameter_change(LFO1_FREQUENCY_TYPE, FIXED);
			break;

		case '1':
			synth_parameter_change(LFO1_FREQUENCY_TYPE, RELATIVE);
			break;

		case '2':
			synth_parameter_change(LFO1_TARGET_TYPE, NONE);
			break;

		case '3':
			synth_parameter_change(LFO1_TARGET_TYPE, FREQUENCY);
			break;

		case '4':
			synth_parameter_change(LFO1_TARGET_TYPE, AMPLITUDE);
			break;

		case '5':
			synth_parameter_change(LFO1_TARGET_TYPE, PULSE_WIDTH);
			break;

		}
	}
}
