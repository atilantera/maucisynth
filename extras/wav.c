#include "wav.h"

/* NOTE: these functions are designed for little-endian systems
 * like Intel x86 */

/* Reads a mono 16-bit 44 kHz WAV file
 * Returns length in samples if successful, -1 otherwise.
 * Allocates *data. */

int read_wav(char * filename, int16_t ** data)
{
  if (filename == NULL || data == NULL)
    return -1;

  FILE * fp;
  fp = fopen(filename, "r");
  if (fp == NULL)
    return -1;

  char idtext[4];
  fread(idtext, 1, 4, fp);
  if (!(idtext[0] == 'R' && idtext[1] == 'I' &&
        idtext[2] == 'F' && idtext[3] == 'F'))
  {
    fclose(fp);
    return -1;
  }

  fseek(fp, 4, SEEK_CUR);
  fread(idtext, 1, 4, fp);
  if (!(idtext[0] == 'W' && idtext[1] == 'A' &&
        idtext[2] == 'V' && idtext[3] == 'E'))
  {
    fclose(fp);
    return -1;
  }
  fread(idtext, 1, 4, fp);
  if (!(idtext[0] == 'f' && idtext[1] == 'm' &&
        idtext[2] == 't' && idtext[3] == ' '))
  {
    fclose(fp);
    return -1;
  }

  // AudioFormat must be 1 (PCM)
  // and NumChannels must be 1 (mono)
  fseek(fp, 4, SEEK_CUR);
  fread(idtext, 1, 4, fp);
  if (!(idtext[0] == 1 && idtext[1] == 0 &&
        idtext[2] == 1 && idtext[3] == 0))
  {
    fclose(fp);
    return -1;
  }

  uint32_t samplerate = 0;
  fread(&samplerate, 4, 1, fp);
  if (samplerate != 44100) {
    fclose(fp);
    return -1;
  }

  fseek(fp, 6, SEEK_CUR);
  fread(idtext, 1, 2, fp);
  if (!(idtext[0] == 16 && idtext[1] == 0))
  {
    fclose(fp);
    return -1;
  }

  fread(idtext, 1, 4, fp);
  if (!(idtext[0] == 'd' && idtext[1] == 'a' &&
        idtext[2] == 't' && idtext[3] == 'a'))
  {
    fclose(fp);
    return -1;
  }

  uint32_t length = 0;
  fread(&length, 4, 1, fp);
  length /= 2; // Every sample has two bytes (16 bits)

  *data = (int16_t *) malloc(length * 2);
  if (*data == NULL) {
    fclose(fp);
    return -1;
  }

  fread(*data, 2, length, fp);

  fclose(fp);
  return length;
}

/* Writes a mono 16-bit 44 kHz WAV file
 * 1 if successful, 0 otherwise.
 * Parameter "length" is in samples. */

int write_wav(char * filename, int16_t * data, uint32_t length)
{
  if (filename == NULL || data == NULL)
    return 0;

  FILE * fp;
  fp = fopen(filename, "w");
  if (fp == NULL)
    return -1;

  fwrite("RIFF", 1, 4, fp);
  uint32_t total_length = length * 2 + 32;
  fwrite(&total_length, 1, 4, fp);
  fwrite("WAVEfmt ", 1, 8, fp);
  
  uint32_t fmt_chunk_length = 0x10;
  fwrite(&fmt_chunk_length, 1, 4, fp);
  uint16_t one = 1;
  fwrite(&one, 1, 2, fp);
  fwrite(&one, 1, 2, fp);

  uint32_t samplerate = 44100;
  fwrite(&samplerate, 4, 1, fp);
  uint32_t bytes_per_second = samplerate * 2;
  fwrite(&bytes_per_second, 4, 1, fp);
  uint16_t bytes_per_sample = 2;
  fwrite(&bytes_per_sample, 2, 1, fp);
  uint16_t bits_per_sample = 16;
  fwrite(&bits_per_sample, 2, 1, fp);

  fwrite("data", 1, 4, fp);
  uint32_t data_length = length * 2;
  fwrite(&data_length, 4, 1, fp);

  fwrite(data, 2, length, fp);

  fclose(fp);
  return 1;
}

