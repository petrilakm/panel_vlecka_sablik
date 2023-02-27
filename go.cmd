@echo off
for /R ".\inc" %%i IN (*.c) DO (
echo.
echo.
echo *** File: %%~nxi ***
avr-gcc -pipe -c -x c -funsigned-char -funsigned-bitfields -DDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g -g2 -Wall -mmcu=atmega128 -Iinc -Wall -Wstrict-prototypes -std=gnu99 -MD -MP inc/%%~ni.c -o obj/%%~ni.o
)
rem for /R ".\inc" %%i IN (*.s) DO (
rem echo.
rem echo.
rem echo *** File asm: %%~nxi ***
rem avr-gcc -pipe -c -x assembler-with-cpp -funsigned-char -funsigned-bitfields -DDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega8 -Iinc -Wall -Wstrict-prototypes -std=gnu99 -MD -MP inc/%%~ni.s -o obj/%%~ni.o
rem )
echo.
echo.
echo *** File: pult_sablik ***
avr-gcc -pipe -c -x c -funsigned-char -funsigned-bitfields -DDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g -g2 -Wall -mmcu=atmega128 -Iinc -Wall -Wstrict-prototypes -std=gnu99 -MD -MP pult_sablik.c -o obj/pult_sablik.o
echo Finish
echo on
