# This is a shitty makefile.
# I'm just sick of waiting to program.

#------------------------------------------------------------------------------
# CONFIGURATION
#-----------------------------------------------------------------------------

TARGET := ARCHON
DEVICE := atmega328p
F_CPU := 16000000
# 0, 1, 2, 3, s, or debug
OPT := 2

# Can be set from command line if different
AVR_PORT ?= /dev/ttyUSB0

AVR_PROGRAMMER := -c arduino -P $(AVR_PORT) -b 57600
# AVR_PROGRAMMER := -c atmelice_isp -B 1

# Programs
CC := avr-gcc
INSTALL := avrdude
OBJCOPY := avr-objcopy
REMOVE := rm -rf

# Directories
# NOTE: Use '.' for no directory
BINDIR := bin
OBJDIR := obj
SRCDIR := src

# Compiler Flags
CFLAGS := -O$(OPT) -DF_CPU=$(F_CPU)UL -g -mmcu=$(DEVICE) -std=gnu11
CFLAGS += -Wl,--gc-sections -mrelax
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fshort-enums
CFLAGS += -fpack-struct

# Programmer Flags
AVRDUDE_FLAGS := -p $(DEVICE) $(AVR_PROGRAMMER)
AVRDUDE_FLASH := -U flash:w:$(BINDIR)/$(TARGET).hex:i
# AVRDUDE_EEPROM := -U eeprom:w:$(BINDIR)/$(TARGET).eep:i

#------------------------------------------------------------------------------
# TARGETS
#------------------------------------------------------------------------------

#-Primary Targets--------------------------------------------------------------

all: begin gccversion build source size end

clean: begin clean_list end

install: $(BINDIR)/$(TARGET).hex
	sudo $(INSTALL) $(AVRDUDE_FLAGS) $(AVRDUDE_FLASH) $(AVRDUDE_EEPROM)

#-Support Targets (PHONY)------------------------------------------------------
begin:
	@echo
	@echo --------begin--------

clean_list:
	@echo Cleaning...
	$(REMOVE) $(BINDIR)
	$(REMOVE) $(OBJDIR)

end:
	@echo ---------end---------
	@echo

gccversion:
	@$(CC) --version

build: $(BINDIR)/$(TARGET).hex

dirs:
	mkdir -p $(BINDIR) $(OBJDIR)

size: $(BINDIR)/$(TARGET).elf
	@echo "Computing size..."
	@avr-size $(BINDIR)/$(TARGET).elf

source: $(BINDIR)/$(TARGET).elf
	@echo "Generating .lst file..."
	@avr-objdump -h -S $(BINDIR)/$(TARGET).elf > $(BINDIR)/$(TARGET).lst

.PHONY: all begin build dirs clean end gccversion size source


#-File Targets (The real work)-------------------------------------------------
# This makefile assumes that all C files have at most, a single corresponding
# header file sharing the same name (e.g. foo.c and foo.h).
#------------------------------------------------------------------------------

# Identify C files - compute object file names.
# Assumes target depends on all c files
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Generate target hex file from ELF
$(BINDIR)/$(TARGET).hex: $(BINDIR)/$(TARGET).elf | dirs
	$(OBJCOPY) -O ihex -R .eeprom $(BINDIR)/$(TARGET).elf $(BINDIR)/$(TARGET).hex

# Link ELF from various object files
$(BINDIR)/$(TARGET).elf: $(OBJ) | dirs
	$(CC) $(CFLAGS) -o $@ $^

# Compile all object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@
