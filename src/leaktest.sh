#!/bin/sh
#valgrind --leak-check=full --suppressions=maucisynth.sup --log-file=vgdump ./maucisynth
valgrind --leak-check=full --log-file=vgdump ./maucisynth
## grep -n "by .*maucisynth)" vgdump > maucisynth-leaks.txt
