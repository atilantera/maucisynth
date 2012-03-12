#include "EventBuffer.h"
#include "SynthGui.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	EventBuffer * eventBuffer = new EventBuffer();
	SynthGui * gui = new SynthGui(eventBuffer);
	gtk_main();
	delete gui;
	delete eventBuffer;
	return 0;
}
