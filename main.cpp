#include "EventBuffer.h"
#include "SoundBuffer.h"
#include "SynthGui.h"


int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	EventBuffer eventBuffer;
	SynthGui gui(eventBuffer);
	SoundBuffer sb(128);

	gtk_main();

	return 0;
}
