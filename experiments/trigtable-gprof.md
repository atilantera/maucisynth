## GCC with no optimization

```
g++ -lm -g -pg trigtable.cpp -o trigtable
./trigtable
gprof trigtable | less
```

```
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ns/call  ns/call  name    
 41.22      0.21     0.21  3000001    70.08    70.08  SoundBuffer::clear()
 23.55      0.33     0.12  1000000   120.13   120.13  SineGenerator::tableSignal3(SoundBuffer&)
 19.63      0.43     0.10  1000000   100.11   100.11  SineGenerator::tableSignal2(SoundBuffer&)
  9.81      0.48     0.05  1000001    50.05    50.05  SineGenerator::hardwareSignal(SoundBuffer&)
```

## GCC with -O1

```
g++ -lm -g -pg trigtable.cpp -o trigtable
./trigtable
gprof trigtable | less
```

```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ns/call  ns/call  name    
 50.05      0.15     0.15  1000000   150.16   150.16  SineGenerator::tableSignal2(SoundBuffer&)
 26.70      0.23     0.08  1000000    80.09    80.09  SineGenerator::tableSignal3(SoundBuffer&)
 16.68      0.28     0.05  3000001    16.68    16.68  SoundBuffer::clear()
  6.67      0.30     0.02  1000001    20.02    20.02  SineGenerator::hardwareSignal(SoundBuffer&)
```

## Conclusion

The running time of sinf() from GNU C library is 25-50% of tabled sine signal.
