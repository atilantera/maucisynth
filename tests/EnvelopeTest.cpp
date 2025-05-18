#include <iostream>
#include "../EnvelopeGenerator.h"

using namespace std;

class EnvelopeTest {
public:
	EnvelopeTest();

	void runAllTests();
	void exceptionTests();
};

void EnvelopeTest::runAllTests() {
	this->exceptionTests();
}

void EnvelopeTest::exceptionTests() {
	EnvelopeGenerator g = EnvelopeGenerator(200, 10);
}

int main() {
    cout << "Running EnvelopeTest" << endl;
    return 0;
}
