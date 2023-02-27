#!/bin/bash

#for /R ".\inc" %%i IN (*.c) DO (
for ifull in inc/*.c
do
    ibase="$(basename $ifull)"
    i="${ibase%.*}"
    echo
    echo
    echo "--- File: $ifull ---"
    avr-gcc -pipe -c -x c -funsigned-char -funsigned-bitfields -DDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega128 -Iinc -Wall -Wstrict-prototypes -std=gnu99 -MD -MP inc/${i}.c -o obj/${i}.o
#)
done
#for ifull in inc/*.s
#do
#    ibase="$(basename $ifull)"
#    i="${ibase%.*}"
#    echo
#    echo
#    echo "--- File asm: $ifull ---"
#    avr-gcc -pipe -c -x assembler-with-cpp -funsigned-char -funsigned-bitfields -DDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega8 -Iinc -Wall -Wstrict-prototypes -std=gnu99 -MD -MP inc/${i}.s -o obj/${i}.o
#)
#done
echo
echo
echo --- File: pult_sablik ---
avr-gcc -pipe -c -x c -funsigned-char -funsigned-bitfields -DDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega128 -Iinc -Wall -Wstrict-prototypes -std=gnu99 -MD -MP pult_sablik.c -o pult_sablik.o
echo Finish
