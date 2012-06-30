#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Box-Muller transform: how to generate pseudorandom numbers from
 * the normal distribution.
 *
 * u and v are random values from uniform distribution whose
 * range is ]0,1]. (Zero is excluded.)
 *
 * After u and v are generated, w and v are random values from the standard
 * normal distribution (expected value = 0, variance = 1):
 *
 * u = sqrt(-2 * log(u)) * cos(2 * pi * v)
 * v = sqrt(-2 * log(u)) * sin(2 * pi * v)
 *
 * http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
 *
 * On 32-bit systems, where RAND_MAX = 2^31 - 1 = 2147483647,
 * it is guaranteed that minimum random value for u,
 * 1 / 2147483647 ~= 4.66 * 10^-10, will not underflow to zero
 * (which would cause an error when calculating log(u)).
 *
 * Producing pseudorandom numbers from normal distribution with parameters
 * (expected value = m, standard deviation = s):
 * 
 * z = (x - m) / s   <=>   x = z * s + m
 *
 * z = normalized value (from Box-Muller transform)
 * x = denormalized value
 *
 * Restricting values from standard normal distribution to range [-a,a],
 * where a > 1:
 *
 * -1 <= cos(2 * pi * v) <= 1  and  -1 <= sin(2 * pi * v) <= 1. Those terms
 * affect mainly on the sign of the result.
 * 1 <= sqrt(-2 * log(u)) < +infinity.
 * Therefore, if we want u and v belong to range [-a,a], it is enough to set
 *     sqrt(-2 * log(u)) < a
 * <=>      -2 * log(u)  < a^2
 * <=>           log(u)  > -0.5 * a^2
 * <=>           u       > e^(-0.5*a^2)
 *
 * With normal distribution having parameters (m, s) and wanting to limit
 * the random values to range [m-b,m+b]:
 *     sqrt(-2 * log(u)) * s < b
 * <=> sqrt(-2 * log(u))     < b / s
 * <=>      -2 * log(u)      < (b/s)^2
 * <=>           log(u)      > -0.5 * (b/s)^2
 * <=>               u       > e^(-0.5 * (b/s)^2)
 * (The expected value m does not affect the width of the range.)
 */

/* Produces two random values (a,b) from normal distribution with parameters
 * (expected value = mu, variance = sigma^2).
 * a, b      random values
 * mu        expected value
 * sigma     standard deviation
 * maxvalue  If maxvalue > 1, then values will be limited to range
 *           [-maxvalue, maxvalue].
 *           Set maxvalue = 0 for no limit.
 */
void boxmuller(float * a, float * b, float mu, float sigma, float maxvalue)
{
    float u, v, R, Cos, Sin, limit, tmp;
    int r;

    if (!(sigma > 0)) {
        return;
    }
    do {
        r = rand();
    } while (r == 0);
    u = (float)r / RAND_MAX;
    
    if (maxvalue > 1) {
        tmp = maxvalue / sigma;
        limit = expf(-0.5 * tmp * tmp);
        u = limit + u / (1 + limit);
        if (u > 1) {
            u = 1;
        }
    } 

   do {
        r = rand();
    } while (r == 0);
    v = (float)r / RAND_MAX;

    R = sqrt(-2 * logf(u));
    sincosf(v * 6.283185308, &Sin, &Cos);

    *a = R * Cos * sigma + mu;
    *b = R * Sin * sigma + mu;
}

void histogram(float * data, int samples, float min, float max, int blocks);

void test1()
{
    int i;
    int histogram[61];
    for (i = 0; i < 61; i++) {
        histogram[i] = 0;
    }

    float a, b;
    int index;
    for (i = 0; i < 500; i++) {
        boxmuller(&a, &b, 0, 1, 0);
        index = a * 10 + 30;
        if (index >= 0 && index <= 60) {
            histogram[index]++;
        }
        index = b * 10 + 30;
        if (index >= 0 && index <= 60) {
            histogram[index]++;
        }
    }

    for (i = 0; i < 61; i++) {
        printf("%f %d\n", (i - 30) * 0.1, histogram[i]);
    }
}

void test2()
{
    int i;
    float a, b;
    float data[1000];
    
    i = 0;
    do {
        boxmuller(&a, &b, 55.166, 0.02638, 0);
        if (a > 55.09 && a < 55.24) {
            data[i++] = a;
        }
        if (i < 1000 && b > 55.09 && b < 55.24) {
            data[i++] = b;
        }
    } while (i < 1000);

    histogram(data, 1000, 55.09, 55.24, 15);
}

void histogram(float * data, int samples, float min, float max, int blocks)
{
    unsigned int hist_table[50];
    int i, index;
    float block_width;

    if (data == NULL || samples < 2 || min > max ||
        blocks < 2 || blocks > 50)
    {
        return;
    }
    for (i = 0; i < blocks; i++) {
        hist_table[i] = 0;
    }
        
    block_width = (max - min) / blocks;

    float * ptr = data;
    float * end_ptr = data + samples;
    while (ptr < end_ptr) {
        index = ((*ptr++) - min) / block_width;
        if (index >= 0 && index < blocks) {
            hist_table[index]++;
        }
    }
    
    float block_center_value = min + block_width / 2;
    for (i = 0; i < blocks; i++) {
        printf("%f %u\n", block_center_value, hist_table[i]);
        block_center_value += block_width;
    }
}

void underflow_test()
{
    float nearzero = 1.0 / RAND_MAX;
    printf("1.0 / %d = %f (= %e)\n", RAND_MAX, nearzero, nearzero);
}

int main()
{
    test2();
    return 0;
}
