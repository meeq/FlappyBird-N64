PROG_NAME = flappy
PROG_TITLE = "Flappy Test"
ROM_SIZE = 2M

OBJS = main.o

# Paths
DFSDIR = ./filesystem/
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header

# Flags
LINK_FLAGS = -L$(ROOTDIR)/lib -L$(ROOTDIR)/mips64-elf/lib -ldragon -lc -lm -ldragonsys -Tn64ld.x
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/include -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300

# Binaries
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

$(PROG_NAME).v64: $(PROG_NAME).elf $(PROG_NAME).dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME).v64
	$(N64TOOL) -b -l $(ROM_SIZE) -t $(PROG_TITLE) -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME).v64 $(PROG_NAME).bin -s 1M $(PROG_NAME).dfs
	$(CHKSUM64PATH) $(PROG_NAME).v64

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $(OBJS) $(LINK_FLAGS)

$(PROG_NAME).dfs:
	find $(DFSDIR) -name ".DS_Store" -depth -exec rm {} \;
	$(MKDFSPATH) $(PROG_NAME).dfs $(DFSDIR)

all: $(PROG_NAME).v64

clean:
	rm -f *.v64 *.elf *.o *.bin *.dfs
