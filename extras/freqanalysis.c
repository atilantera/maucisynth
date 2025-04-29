/*
 * Tämän softan on tarkoitus toimia tarkan kitaranvirittimen tavoin:
 * kertoa, miten läheltä avaintaajuutta, kuten 55 Hz, signaalista löytyy
 * vastaava taajuus, esim. 55,01 Hz.
 *
 * Tällä hetkellä ongelmana on pyöristysvirheet:
 * täydellisen siniaallon 55 Hz ohjelma arvaa väärin 55,02 Hz:ksi.
 * Siis näytteistystaajuus on 44100 Hz, jolloin yksi 55 Hz jakso on
 * 801,818181... näytettä. 40 jaksolla näytteitä on 32072,7272...
 * Jos mittaisi huipusta huippuun, niin tarkkuus on:
 * 44100/(32072/40) = 55,00125 Hz
 * 44100/(32073/40) = 54,99953 Hz
 * Teoreettinen minimivirhe tällä taajuudella, ja näytteistyksellä ja 40
 * jakson mittausikkunalla on siis kokoluokkaa 32073/32072 ~= 1,000031
 * eli voidaan sanoa, että virhe on enintään 0,0001.
 */

#define _GNU_SOURCE
#include <math.h>
#include <string.h>
#include "wav.h"

#define SAMPLERATE 44100

static char * wav_file_name = NULL;
static int16_t * wav_data = NULL;
static int wav_data_length = 0;

int handle_args(int argc, char ** argv);
void menu();
void menu_findpeak();
void menu_mag();
void menu_nearf();
void menu_nearfpeak();
void menu_nearftest();
void menu_printaudio();

void print_audio_data(int16_t * data, int first_sample, int last_sample,
    int sample_marker);

int peak(int16_t * data, int first, int last);

float
fourier_magnitude(int16_t * data, int first, int cycles, float frequency);

void
frequency_search(int16_t * data, int first_sample, int cycles,
float frequency, float range_width, int iterations);

void
frequency_search_test(int16_t * data, int first_sample, int cycles,
float frequency, float freq_range_width, int freq_points);

float
frequency_search_by_peak(int16_t * data, int first_peak, int cycles,
float frequency, int print_peaks);

int main(int argc, char ** argv)
{
    if (handle_args(argc, argv) == 0) {
        return 1;
    }

    wav_data_length = read_wav(wav_file_name, &wav_data);
    if (wav_data_length == -1) {
        printf("Cannot read wav file. Format must be 16 bit 44100 Hz mono"
        " WAV.\n");
    }

    printf("Read %d samples (%f seconds).\n", wav_data_length,
        (float)wav_data_length / 44100);

    menu();

    free(wav_data);

    return 0;
}

int handle_args(int argc, char ** argv)
{
    if (argc != 2) {
        printf("Usage: %s <sound.wav>\n", argv[0]);
        return 0;
    }
    wav_file_name = argv[1];
    return 1;
}

void menu()
{
    printf("Menu:\n"
        "mag        - find magnitude of a sine frequency\n"
        "nearf      - find frequency near given frequency\n"
        "nearfpeak  - find frequency near given frequency (peak-to-peak)\n"
        "nearftest  - list frequency magnitudes near given frequency\n"
        "peak       - find peak value in audio range\n"
        "printaudio - print waveform\n"
        "quit       - quit the program\n");

    char command[21];
    while (1) {
        printf("\ncommand: ");
        scanf("%20s", command);
        if (strcmp(command, "mag") == 0) {
            menu_mag();
            continue;
        }
        if (strcmp(command, "nearf") == 0) {
            menu_nearf();
            continue;
        }
        if (strcmp(command, "nearfpeak") == 0) {
            menu_nearfpeak();
            continue;
        }
        if (strcmp(command, "nearftest") == 0) {
            menu_nearftest();
            continue;
        }
        if (strcmp(command, "peak") == 0) {
            menu_findpeak();
            continue;
        }
        if (strcmp(command, "printaudio") == 0) {
            menu_printaudio();
            continue;
        }
        if (strcmp(command, "quit") == 0) {
            return;
        }
    }
}

void menu_findpeak()
{
    float range_start, range_end;
    int peak_in_samples;
    printf("Range start (seconds): ");
    scanf("%f", &range_start);
    printf("Range end (seconds): ");
    scanf("%f", &range_end);
  
    printf("Searching in samples [%d, %d].\n",
        (int)(range_start * SAMPLERATE),
        (int)(range_end * SAMPLERATE));
    peak_in_samples = peak(wav_data, range_start * SAMPLERATE,
        range_end * SAMPLERATE);

    printf("Peak: %f seconds.\n", (float)peak_in_samples / SAMPLERATE);
}

void menu_mag()
{
    float frequency, time_start;
    int cycles;
    printf("Range start time (seconds): ");
    scanf("%f", &time_start);
    printf("Cycles: ");
    scanf("%d", &cycles);
    printf("Frequency to search (Hz): ");
    scanf("%f", &frequency);

    printf("%f\n", fourier_magnitude(wav_data, time_start * SAMPLERATE,
        cycles, frequency));
}

void menu_nearf()
{
    float frequency, time_start, frequency_range_width;
    int cycles;

    printf("Range start time (seconds): ");
    scanf("%f", &time_start);
    printf("Cycles: ");
    scanf("%d", &cycles);
    printf("Frequency to search (Hz): ");
    scanf("%f", &frequency);
    printf("Width of frequency range (Hz): ");
    scanf("%f", &frequency_range_width);

    frequency_search(wav_data, time_start * SAMPLERATE, cycles, frequency,
        frequency_range_width, 10);
}

void menu_nearfpeak()
{
    float frequency, time_start;
    int cycles;
    printf("Time before first peak (seconds): ");
    scanf("%f", &time_start);
    printf("Cycles: ");
    scanf("%d", &cycles);
    printf("Frequency to search (Hz): ");
    scanf("%f", &frequency);

    printf("%f\n",
        frequency_search_by_peak(wav_data, time_start * SAMPLERATE, cycles,
        frequency, 1));
}

void menu_nearftest()
{
    float frequency, time_start, frequency_range_width;
    int cycles, frequency_points;

    printf("Range start time (seconds): ");
    scanf("%f", &time_start);
    printf("Cycles: ");
    scanf("%d", &cycles);
    printf("Frequency to search (Hz): ");
    scanf("%f", &frequency);
    printf("Width of frequency range (Hz): ");
    scanf("%f", &frequency_range_width);
    printf("Number of frequencies in the range: ");
    scanf("%d", &frequency_points);

    frequency_search_test(wav_data, time_start * SAMPLERATE, cycles, frequency,
        frequency_range_width, frequency_points);
}

void menu_printaudio()
{
    float time_start, time_end, marker;

    printf("Range start time (seconds): ");
    scanf("%f", &time_start);

    printf("Range end time (seconds): ");
    scanf("%f", &time_end);

    printf("Marker time (seconds): ");
    scanf("%f", &marker);

    print_audio_data(wav_data, time_start * SAMPLERATE, time_end * SAMPLERATE,
        marker * SAMPLERATE);
}
  
/* Finds position of highest sample value in the audio range.
 * data - signed 16-bit mono audio
 * first - first sample in the range
 * last - last sample in the range
 */
int peak(int16_t * data, int first, int last)
{
    int16_t max = data[first];
    int max_i = first;
    int i;
    last++;
    for (i = first + 1; i < last; i++) {
        if (data[i] > max) {
            max = data[i];
            max_i = i;
        }
    }
    return max_i;
}

/* Returns magnitude [0..1] of given frequency in the audio signal
 * measured with Discrete Fourier Transform.
 * data - signed 16-bit mono audio
 * first - first sample in the range
 * cycles - window length in cycles regarding to frequency
 * frequency - base frequency for which to find Fourier coefficents
 *
 * NOTE: There is some error in the magnitude because of the discrete
 * calculation. error ~ frequency / cycles: higher frequencies have less
 * samples in one cycle. Less samples -> less data -> less precision.
 * For extreme cases, like 440 Hz sine wave at 44100 Hz sample rate and
 * amplitude of 1, the magnitude is even slightly over 1.
 */
float
fourier_magnitude(int16_t * data, int first, int cycles, float frequency)
{
    int range_length = SAMPLERATE / frequency * cycles;

    /* 1st order discrete Fourier coefficents for frequency f:
     *
     * a_1 = 2 * sum[x=a..b]( 1/range_length * audio[x] * cos(k*x) )
     *     = 2/range_length * sum[x=a..b]( audio[x] * cos(k*x) )
     * b_1 = 2/range_length * sum[x=a..b]( audio[x] * sin(k*x) )
     * k = angle_per_sample
     *
     * C (variable "sum_coefficent"):
     * when b-a = 2 * pi, C = 1/pi, because
     * integrate[0..2*pi] sin(x) * sin(x) dx = pi.
     *
     * (Nondiscrete version:
     * integral[x=-pi..pi] sin(x)*sin(x) dx = pi = (1 * 2*pi) / 2
     * because of symmetry.)
     */

    /* Convert from 16-bit signed int sample values [-32768..32767]
     * to 32-bit float values[-1..1]. */
    float sample_coefficent = 1.0 / 32768;
  
    float angle = 0;
    float angle_per_sample = 2 * 3.141592654 * frequency / SAMPLERATE;
    

    float sample;
    float cosine, sine, cosine_sum, sine_sum;
    sine_sum = 0;
    cosine_sum = 0;

    int16_t * ptr = data + first;
    int16_t * end_ptr = ptr + range_length;
    while (ptr < end_ptr) {
        sincosf(angle, &sine, &cosine);
        sample = (*ptr++) * sample_coefficent;

        sine_sum += sample * sine;
        cosine_sum += sample * cosine;
        angle += angle_per_sample;
    }
    float a = 2 * cosine_sum / range_length;
    float b = 2 * sine_sum / range_length;
    return sqrt(a * a + b * b);
}

/* Finds a frequency in the audio signal that is closest to a given
 * frequency. Uses range splitting.
 * data - signed 16-bit mono audio
 * first - first sample in the range
 * cycles - window length in cycles regarding to frequency
 * frequency - base frequency for which to find Fourier coefficents (Hz)
 * range_width - initial range (Hz):
 * iterations - how many times to split the range.
 *
 * iterations = 10 gives frequency/1000 accuracy,
 * iterations = 20 gives frequency/1000000 accuracy etc.
 * Example: If frequency = 440 Hz and iterations = 10,
 * the result is at nearest 440/1000 Hz = 0.44 Hz.
 */

void
frequency_search(int16_t * data, int first_sample, int cycles, float frequency,
float range_width, int iterations)
{
    float range_left, test_frequency, test_frequency_increase;
    float magnitude, max_magnitude, freq_max_magnitude;
    int i, j;

    /* Find magnitudes of 11 test frequencies in current range.
     * Center for new range is the test frequency with highest magnitude
     * in the data. The width of the new range is half of the current range.
     */

    range_left = frequency - range_width * 0.5;
    for (i = 0; i < iterations; i++) {
        test_frequency = range_left;        
        test_frequency_increase = range_width * 0.1;
        max_magnitude = 0;
        for (j = 0; j < 11; j++) {
            magnitude = fourier_magnitude(data, first_sample, cycles,
                test_frequency);
            printf("[%d]: %f %f\n", j, test_frequency, magnitude);
            if (magnitude > max_magnitude) {
                max_magnitude = magnitude;
                freq_max_magnitude = test_frequency;
            }
            test_frequency += test_frequency_increase;
        }
        range_width *= 0.5;
        range_left = freq_max_magnitude - range_width * 0.5;
    }
}

/* Finds magnitudes of test frequencies at constant intervals in a frequency
 * range aroud given frequency.
 * data - signed 16-bit mono audio
 * first - first sample in the range
 * cycles - window length in cycles regarding to frequency
 * frequency - key frequency for which to find first order Fourier
 * coefficents (Hz)
 * freq_range_width - initial frequency range (Hz):
 * freq_points - how many test frequencies
 */
void
frequency_search_test(int16_t * data, int first_sample, int cycles,
float frequency, float freq_range_width, int freq_points)
{
    float freq_increase = freq_range_width / (freq_points - 1);
    float test_frequency = frequency - freq_range_width / 2;

    float magnitude, max_magnitude, freq_max_magnitude;
    max_magnitude = 0;

    int i;
    for (i = 0; i < freq_points; i++) {
        magnitude = 
            fourier_magnitude(data, first_sample, cycles, test_frequency);
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            freq_max_magnitude = test_frequency;
        }
        printf("%f %f\n", test_frequency,
            fourier_magnitude(data, first_sample, cycles, test_frequency));
        test_frequency += freq_increase;
    }
    printf("frequency at max magnitude: %f\n", freq_max_magnitude);
}

/* Finds a frequency in the audio signal that is closest to a given
 * frequency. Measures cycle time from positive peak to positive peak
 * between two cycles.
 * data - signed 16-bit mono audio
 * first_peak - sample position near the first peak.
 * cycles - over how many cycles the frequency is measured.
 * frequency - key frequency for which to find the nearest frequency
 *
 * NOTE: Assumption for the data is that there is a dominant positive peak
 * in the same position of every cycle.
 */
float
frequency_search_by_peak(int16_t * data, int first_peak, int cycles,
float frequency, int print_peaks)
{
    int peak_window_length = 0.9 * SAMPLERATE / frequency;
    int peak_b_position = first_peak + SAMPLERATE / frequency * cycles;

    int peak_a = peak(data, first_peak, first_peak + peak_window_length);
    int peak_b = peak(data, peak_b_position,
                      peak_b_position + peak_window_length);

    int sample_span = peak_b - peak_a;

    if (print_peaks == 1) {
        print_audio_data(data, first_peak, first_peak + peak_window_length,
            peak_a);
        print_audio_data(data, peak_b_position,
            peak_b_position + peak_window_length, peak_b);
    }

    return SAMPLERATE / (sample_span / (float)cycles);
}

/*
 * Prints a portion of audio as character graphics.
 * 70 x 22 characters.
 */
void
print_audio_data(int16_t * data, int first_sample, int last_sample,
int sample_marker)
{
    int i;
    char text_buffer[1563];
    float sample_values[70];
    int position;
    float value;
    int16_t * ptr;
    int16_t * end_ptr;

    /* text buffer is 71 * 22 characters. */
    memset(text_buffer, ' ', 710);
    memset(text_buffer + 710, '-', 70);
    memset(text_buffer + 781, ' ', 781);
    for (i = 0; i < 22; i++) {
        text_buffer[i * 71 + 70] = '\n';
    }
    text_buffer[1562] = '\0';

    int samples_in_window = last_sample + 1 - first_sample;
    int samples_per_char = samples_in_window / 70;
    if (samples_per_char == 0) {
        samples_per_char = 1;
    }

    for (i = 0; i < 70; i++) {
        value = 0;
        ptr = data + first_sample + (int)((float)i / 69 * samples_in_window);
        end_ptr = ptr + samples_per_char;
        while (ptr < end_ptr) {
            value += (*ptr++);
        }
        sample_values[i] = value / (samples_per_char * 32768);
    }


    float max_value = 0;
    float abs_value;
    for (i = 0; i < 70; i++) {
        abs_value = sample_values[i];
        if (abs_value < 0) {
            abs_value = 0 - abs_value;
        }
        if (abs_value > max_value) {
            max_value = abs_value;
        }
    }

    float normalization = 10 / max_value;
    for (i = 0; i < 70; i++) {
        position = 10 - roundf(sample_values[i] * normalization);
        if (position < 0) {
            position = 0;
        }
        if (position > 20) {
            position = 20;
        }
        text_buffer[position * 71 + i] = '+';
    }

    if (sample_marker >= first_sample && sample_marker <= last_sample) {
        position = (float)(sample_marker - first_sample) /
                   samples_in_window * 70;
        text_buffer[1491 + position] = 'P';
    }
    printf("%s\n", text_buffer);
}


