#include "EventBuffer.h"
#include "SynthGui.h"
#include "Synthesizer.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	EventBuffer eventBuffer;
	SynthGui gui(eventBuffer);
	Synthesizer synth(eventBuffer);

	if (synth.isActive()) {
		gtk_main();
	}

	return 0;
}
