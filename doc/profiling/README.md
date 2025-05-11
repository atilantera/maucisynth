# Performance profiling for maucisynth

## Using gprof

[GNU gprof](https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_mono/gprof.html)

$ sudo apt install binutils

Version used when writing this documentation:
```
$ gprof --version
GNU gprof (GNU Binutils for Debian) 2.44
Based on BSD gprof, copyright 1983 Regents of the University of California.
This program is free software.  This program has absolutely no warranty.
```

By default, Makefile has gprof support enabled
```
CC=g++
LD=g++
CFLAGS= -std=c++0x -Wall -Wextra -pedantic -march=native -O2 -g -pg \
        `pkg-config --cflags gtk+-2.0` 
LIBS=-ljack -lm -pg `pkg-config --libs gtk+-2.0`
```
I.e., we are using g++, the c++ compiler of GCC. Parameters `-g` and `-pg`
are required for profiling: generate debug information and profiling
information, respectively.
Also the LIBS variable must have `-pg`, because the makefile instructs g++
compile one .cpp file at a time and then later links all compiled object
files (.o) together.

Then run the synthesizer and play some notes:
```
$ ./maucisynth
```

Finally, run gprof_report.sh.:
```
./gprof_report.sh
```

Then, the gprof_report.txt contains profiling data:
```
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 19.90      0.77     0.77   432605     0.00     0.00  LowpassFilter::doFiltering(float*, unsigned int)
 17.06      1.43     0.66   320410     0.00     0.00  MainOscillator::synthesizeTriangleWave(float*, float const*, unsigned int, unsigned int)
 10.08      1.82     0.39   300468     0.00     0.00  LowFrequencyOscillator::generateSound(float*, unsigned int, unsigned int)
  8.27      2.14     0.32   224642     0.00     0.00  MainOscillator::applyRelease(float*, unsigned int, unsigned int)
  7.62      2.44     0.30   432605     0.00     0.01  Synthesizer::generateSound(unsigned int)
  4.65      2.62     0.18   192719     0.00     0.00  MainOscillator::synthesizeSawtoothWave(float*, float const*, unsigned int, unsigned int)
  4.39      2.79     0.17   432605     0.00     0.00  Synthesizer::processGuiEvents()
  4.14      2.95     0.16   668096     0.00     0.00  Synthesizer::renderNote(OscillatorGroup*, bool, float*, unsigned int, unsigned int)
  4.14      3.11     0.16   668096     0.00     0.00  MainOscillator::generateSound(float*, float const*, unsigned int, unsigned int, bool&)
  3.49      3.24     0.14   432605     0.00     0.00  Synthesizer::checkJackExceptions()
...
```


