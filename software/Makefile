# This is a shitty makefile.
# I'm just sick of waiting to program.

#------------------------------------------------------------------------------
# CONFIGURATION
#-----------------------------------------------------------------------------

CC=avr-gcc
OBJCOPY=avr-objcopy
REMOVE=rm -f

CFLAGS=-O2 -DF_CPU=16000000UL -g -mmcu=atmega328p

#------------------------------------------------------------------------------
# TARGETS
#------------------------------------------------------------------------------

#-Primary Targets--------------------------------------------------------------

all: begin gccversion build end

clean: begin clean_list end

install: ARCHON.hex
	avrdude -p m328p -c arduino -P /dev/ttyUSB0 -b 57600 -U flash:w:ARCHON.hex:i

#-Support Targets (PHONY)------------------------------------------------------

begin:
	@echo
	@echo --------begin--------

clean_list:
	@echo Cleaning...
	${REMOVE} *.elf
	${REMOVE} *.hex
	${REMOVE} *.lst
	${REMOVE} *.o

end:
	@echo ---------end---------
	@echo

gccversion:
	@${CC} --version

build: ARCHON.hex

.PHONY: all begin build clean end gccversion


#-File Targets (The real work)-------------------------------------------------

ARCHON.hex: ARCHON.elf
	${OBJCOPY} -O ihex -R .eeprom ARCHON.elf ARCHON.hex

ARCHON.elf: ARCHON.o COMM.o UART.o
	${CC} ${CFLAGS} -o ARCHON.elf ARCHON.o COMM.o UART.o

ARCHON.o: src/ARCHON.c
	${CC} ${CFLAGS} -c src/ARCHON.c

COMM.o: src/COMM.c src/COMM.h
	${CC} ${CFLAGS} -c src/COMM.c

UART.o: src/UART.c src/UART.h
	${CC} ${CFLAGS} -c src/UART.c
