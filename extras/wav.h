#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* NOTE: these functions are designed for little-endian systems
 * like Intel x86. */

/* Reads a mono 16-bit 44 kHz WAV file
 * Returns length in samples if successful, -1 otherwise.
 * Allocates *data. */

int read_wav(char * filename, int16_t ** data);

/* Writes a mono 16-bit 44 kHz WAV file
 * 1 if successful, 0 otherwise.
 * Parameter "length" is in samples. */

int write_wav(char * filename, int16_t * data, uint32_t length);

#endif
