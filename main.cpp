#include "EventBuffer.h"
#include "SynthGui.h"
#include "Synthesizer.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	EventBuffer eventBuffer;
	Synthesizer synth(eventBuffer);

	if (synth.isActive()) {
		SynthGui gui(eventBuffer);
		gtk_main();
	}

	return 0;
}
