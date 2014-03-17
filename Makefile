PROG_NAME = flappy
PROG_TITLE = "Flappy Test"
ROM_SIZE = 2M

# Paths
DFSDIR = filesystem/
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header

# Code files
SRCDIR = src/
C_FILES = $(wildcard $(SRCDIR)*.c)
OBJS = $(subst .c,.o,$(C_FILES))

# Audio files
AIFFDIR = resources/sfx/
AIFF_FILES = $(wildcard $(AIFFDIR)*.aiff)
PCMDIR = $(DFSDIR)sfx/
PCM_TMP = $(subst $(AIFFDIR),$(PCMDIR),$(AIFF_FILES))
PCM_FILES = $(subst .aiff,.raw,$(PCM_TMP))

# Sprite files
PNGDIR = resources/gfx/
PNG_FILES = $(wildcard $(PNGDIR)*.png)
SPRITEDIR = $(DFSDIR)gfx/
SPRITE_TMP = $(subst $(PNGDIR),$(SPRITEDIR),$(PNG_FILES))
SPRITE_FILES = $(subst .png,.sprite,$(SPRITE_TMP))

# N64 Emulator
MESSEMUDIR = $(N64_INST)/../mess0152-64bit
MESSEMUPATH = $(MESSEMUDIR)/mess64

# Flags
LINK_FLAGS = -L$(ROOTDIR)/lib -L$(ROOTDIR)/mips64-elf/lib -ldragon -lm -lc -ldragonsys -Tn64ld.x
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/include -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300

# Binaries
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

# Compilation pipeline

# ROM Image
$(PROG_NAME).v64: $(PROG_NAME).elf $(PROG_NAME).dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME).v64
	$(N64TOOL) -b -l $(ROM_SIZE) -t $(PROG_TITLE) -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME).v64 $(PROG_NAME).bin -s 1M $(PROG_NAME).dfs
	$(CHKSUM64PATH) $(PROG_NAME).v64

# Linked binary
$(PROG_NAME).elf: $(OBJS)
	$(LD) -o $(PROG_NAME).elf $(OBJS) $(LINK_FLAGS)

# Filesystem pipeline

# Sprites
$(SPRITE_FILES): $(PNG_FILES)
	sh ./convert_gfx.sh $?

# PCM Audio
$(PCM_FILES): $(AIFF_FILES)
	sh ./convert_sfx.sh $?

# DragonFS file
$(PROG_NAME).dfs: $(SPRITE_FILES) $(PCM_FILES)
	find $(DFSDIR) -name ".DS_Store" -depth -exec rm {} \;
	$(MKDFSPATH) $(PROG_NAME).dfs $(DFSDIR)

# Housekeeping

all: $(PROG_NAME).v64

emulate:
	sh ./run_emulator.sh $(shell pwd)/$(PROG_NAME).v64

clean:
	rm -Rf $(DFSDIR)
	rm -f *.v64 *.elf src/*.o *.bin *.dfs

.PHONY: all clean emulate
