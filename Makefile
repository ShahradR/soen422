ARCH = at90usb1286
CC = avr-gcc
OC = avr-objcopy
TL = teensy_loader_cli

CFLAGS = -mmcu=$(ARCH) -O -o
OFLAGS = -O ihex
TLFLAGS = -mmcu=$(ARCH) -w

ODIR = ./obj
HDIR = ./hex

CFILES = $(wildcard ./src/*.c)
IFILES = $(wildcard ./include/**/*.c)
OFILES = $(addprefix ./obj/, $(notdir $(CFILES:.c=.o)))
HFILES = $(addprefix ./hex/, $(notdir $(CFILES:.c=.hex)))

MAIN_FILE = blinky

compile: $(OFILES)

hex: compile $(HFILES)

load: hex $(HFILE)
	$(TL) $(TLFLAGS) $(HDIR)/$(MAIN_FILE).hex

clean:
	rm -rf ./obj
	rm -rf ./hex

./obj/%.o: ./src/%.c
	mkdir -p $(ODIR)
	$(CC) $(CFLAGS) $@ $< $(IFILES)	

./hex/%.hex: ./obj/%.o
	mkdir -p $(HDIR)
	$(OC) $(OFLAGS) $< $@ 