MCU=atmega328p
BAUD=9600
BAUD_UPLOAD=115200
PORT=/dev/ttyACM0
SRC=crt1.S main.c math_util.c uart.c
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
	$(DUDE) -V -p$(MCU) -carduino -P$(PORT) -b$(BAUD_UPLOAD) -D -Uflash:w:$<:i

monitor:
	screen $(PORT) $(BAUD)

simulate: $(TARGET).elf
	@echo "Run './debug.sh on separate window!"
	simavr -g -m $(MCU) $<

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).map

.PHONY: all upload monitor clean simulate debug
