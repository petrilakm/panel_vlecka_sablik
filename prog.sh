#!/bin/bash
avrdude -c stk500v2 -P /dev/ttyUSB0 -p m128 -U flash:w:pult_sablik.hex:i
#avrdude -P /dev/ttyUSB0 -c STK500v2 -p m8 -U lfuse:w:0x3f:m -U hfuse:w:0xd1:m
#avrdude -P /dev/ttyUSB0 -c STK500v2 -p m8 -U flash:w:BR101.hex:i -y

