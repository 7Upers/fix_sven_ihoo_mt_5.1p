F_CPU = 16000000UL
CC = /usr/bin/avr-gcc
#-Wall enable all warnings
#-mcall-prologues convert code of functions (binnary will be smaller)
CFLAGS = -Os -Wall -mcall-prologues -mmcu=atmega328
#some defines
CFLAGS += -DF_CPU=$(F_CPU)
OBJ2HEX = /usr/bin/avr-objcopy
UISP = /usr/bin/avrdude
FLASHER = arduino
PORT = /dev/ttyUSB0
SPEED = 57600
TARGET = main

main.hex : $(TARGET).elf
	@echo 'converting'
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET).elf $(TARGET).hex

main.elf : $(TARGET).o i2chw.o remote.o sj2323.o sj2258.o
	@echo 'linking'
	$(CC) $(CFLAGS) -o $(TARGET).elf $(TARGET).o i2chw.o remote.o sj2323.o sj2258.o

remote.o : remote.c
	@echo 'remote'
	$(CC) $(CFLAGS) -c -o remote.o remote.c

sj2323.o : sj2323.c
	@echo 'compilling'
	$(CC) $(CFLAGS) -c -o sj2323.o sj2323.c

sj2258.o : sj2258.c
	@echo 'compilling'
	$(CC) $(CFLAGS) -c -o sj2258.o sj2258.c

i2chw.o : i2chw/twimaster.c
	@echo 'i2c'
	$(CC) $(CFLAGS) -c -o i2chw.o i2chw/twimaster.c

main.o : $(TARGET).c
	@echo 'compilling'
	$(CC) $(CFLAGS) -c -o $(TARGET).o $(TARGET).c

prog_a : $(TARGET).hex
	@echo 'flashing'
	$(UISP) -F -V -c $(FLASHER) -P $(PORT) -b $(SPEED) -p m328p -U flash:w:$(TARGET).hex:a

prog : $(TARGET).hex
	@echo 'flashing'
	$(UISP) -F -V -c usbasp -p m328p -U flash:w:$(TARGET).hex:a

test1.o : test1.c
	@echo 'compilling test1'
	$(CC) $(CFLAGS) -c -o test1.o test1.c

test1.elf : test1.o
	@echo 'linking test1'
	$(CC) $(CFLAGS) -o test1.elf test1.o

test1.hex : test1.elf
	@echo 'converting test1'
	$(OBJ2HEX) -R .eeprom -O ihex test1.elf test1.hex

test1 : test1.hex
	@echo 'flashing test1'
	$(UISP) -F -V -c $(FLASHER) -P $(PORT) -b $(SPEED) -p m8 -U flash:w:test1.hex:a

clean :
	@echo 'cleaning'
	rm -f *.hex *.elf *.o

.SILENT: fuse
fuse:
	@echo -e 'get fuse bits\nhfuse\nlfuse\nefuse'
	$(UISP) -F -V -c $(FLASHER) -P $(PORT) -b $(SPEED) -p m8 -U hfuse:r:-:h -U lfuse:r:-:h -U efuse:r:-:h 2>/dev/null
