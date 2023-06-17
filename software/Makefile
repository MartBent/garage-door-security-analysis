run:
	avr-gcc -Os -Wall -o main -mmcu=attiny85 main.c
	avr-objcopy -R .eeprom -O ihex main main.hex;
	avrdude -c usbasp -p attiny85 -B 200kHz -U main.hex;
	rm main.hex main;
