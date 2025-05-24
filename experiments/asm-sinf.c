#include <iostream>
#include <cmath>

using namespace std;

inline float LD_SIN(float __X) {
    register float __VALUE;

    __asm__ __volatile__(
        "fsin            \n\t"
        : "=t" (__VALUE)
        : "0" (__X)
    );
    return __VALUE;
}

void test_glibc_sin(int n) {
    float x = 0;
    for (int i = 0; i < n; i++) {
        sinf(x);
        x += 0.0001;
    }
}

void test_hw_sin(int n) {
    for (int i = 0; i < n; i++) {
        LD_SIN(0.01);
    }
}

void test_hw_sin_2(int n) {
    register float __VALUE;
    float __X;
    float x = 0;
    for (int i = 0; i < n; i++) {
        __X = x;
        __asm__ __volatile__(
            "fsin            \n\t"
            : "=t" (__VALUE)
            : "0" (__X)
        );
        x += 0.0001;
    }
}


int main() {
    cout << "glibc sin(0.2): " << sinf(0.2) << endl;
    cout << "assembler sinf(0.2): " << LD_SIN(0.2) << endl;

    int rounds = 10000000;
    test_glibc_sin(rounds);
    test_hw_sin_2(rounds);

    return 0;
}
 

