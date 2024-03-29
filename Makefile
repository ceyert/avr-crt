MCU=atmega328p
F_CPU=16000000UL
BAUD=115200
PORT=/dev/ttyACM0
SRC=main.c crt1.S math_util.c  
OBJCOPY=avr-objcopy
CC=avr-gcc
DUDE=avrdude
CFLAGS=-mmcu=$(MCU) -nodevicelib -nodefaultlibs -nostdlib -O3 -g0
TARGET=main

all: $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ -Wl,-Map,$(TARGET).map

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(TARGET).hex
	$(DUDE) -V -p$(MCU) -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$<:i

monitor:
	screen $(PORT) 9600

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).map

.PHONY: all upload monitor clean
