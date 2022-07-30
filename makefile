avr-gcc -mmcu=atmega16 -Os -Wall prog.c -o prog.o;
avr-objcopy -O ihex prog.o prog.hex;
sudo avrdude -p m16 -P avrdoper -c usbasp -U flash:w:prog.hex

