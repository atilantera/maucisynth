#include "EventBuffer.h"
#include "SynthGui.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	EventBuffer eventBuffer;
	SynthGui gui(eventBuffer);

	gtk_main();

	return 0;
}
