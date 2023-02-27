#!/bin/bash

echo
rm -f pult_sablik.elf
echo
echo --- Linking ---
avr-gcc -mmcu=atmega128 obj/timer.o obj/xn_stack.o obj/xn_accessory.o obj/logic.o obj/xpressnet.o obj/inputs.o obj/pult_sablik.o -o obj/pult_sablik.elf -Wl,--start-group -Wl,--end-group -Wl,--gc-sections
echo Finish
echo
"avr-objcopy" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "obj/pult_sablik.elf" "pult_sablik.hex"
"avr-objcopy" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "obj/pult_sablik.elf" "pult_sablik.eep"
if [ -f obj/pult_sablik.elf ]; then
    avr-size --format=berkeley obj/pult_sablik.elf
fi

