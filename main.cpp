#include "EventBuffer.h"
#include "SynthGui.h"
#include "Synthesizer.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);

	EventBuffer eventBuffer;
	SynthParameters parameters;

	Synthesizer synth(eventBuffer, parameters);
	SynthGui gui(eventBuffer, parameters);

	if (synth.isActive()) {
		gtk_main();
	}

	return 0;
}
