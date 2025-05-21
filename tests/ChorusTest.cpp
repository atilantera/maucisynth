#include <cassert>
#include <iostream>
#include "../ChorusEffect.h"

using namespace std;

bool arrayEquals(float * arr1, float * arr2, int len) {
    for (int i = 0; i < len; i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }
    return true;
}

void testRingBufferInit() {
    // Ring buffer is initialized properly
    RingBuffer r(4);
    assert(r.length == 4);
    float testArray[] = {0, 0, 0, 0};
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.nextIn == 0);
    assert(r.nextOut == 3);
}

void testRingBufferPut() {
    // Ring buffer put function works as expected
    RingBuffer r(4);

    assert(r.length == 4);
    assert(r.nextIn == 0);
    assert(r.nextOut == 3);

    r.put(7);
    float testArray[] = {7, 0, 0, 0};
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.length == 4);
    assert(r.nextIn == 1);
    assert(r.nextOut == 3);

    r.put(8);
    float testArray2[] = {7, 8, 0, 0};
    assert(arrayEquals(r.samples, testArray2, 4));
    assert(r.length == 4);
    assert(r.nextIn == 2);
    assert(r.nextOut == 3);

    r.put(9);
    float testArray3[] = {7, 8, 9, 0};
    assert(arrayEquals(r.samples, testArray3, 4));
    assert(r.length == 4);
    assert(r.nextIn == 3);
    assert(r.nextOut == 3);

    r.put(10);
    float testArray4[] = {7, 8, 9, 10};
    assert(arrayEquals(r.samples, testArray4, 4));
    assert(r.length == 4);
    assert(r.nextIn == 0);
    assert(r.nextOut == 3);

    r.put(11);
    float testArray5[] = {11, 8, 9, 10};
    assert(arrayEquals(r.samples, testArray5, 4));
    assert(r.length == 4);
    assert(r.nextIn == 1);
    assert(r.nextOut == 3);
}

void testRingBufferGet() {
    // Ring buffer put function works as expected
    RingBuffer r(4);

    assert(r.length == 4);
    assert(r.nextIn == 0);
    assert(r.nextOut == 3);

    for (int i = 7; i < 11; i++) {
        r.put(i);
    }
    float testArray[] = {7, 8, 9, 10};
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.length == 4);
    assert(r.nextIn == 0);
    assert(r.nextOut == 3);

    assert(r.get() == 10);
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.nextOut == 0);

    assert(r.get() == 7);
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.nextOut == 1);

    assert(r.get() == 8);
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.nextOut == 2);

    assert(r.get() == 9);
    assert(arrayEquals(r.samples, testArray, 4));
    assert(r.nextOut == 3);
}


int main() {
    cout << "Chorus effect test" << endl;
    testRingBufferInit();
    testRingBufferPut();
    testRingBufferGet();

    return 0;
}
