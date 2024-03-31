#!/bin/bash

chmod +x debug.sh
avr-gdb -x debug.gdb main.elf
