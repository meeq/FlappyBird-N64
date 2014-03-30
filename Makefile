PROG_NAME = FlappyBird
PROG_TITLE = "FlappyBird64"
# OUT_SIZE = 52672B HEADER + 1M ROM
OUT_SIZE = 1052672B
DFS_OFFSET = 256K

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
OBJS := $(C_FILES:.c=.o)
DEPS := $(OBJS:.o=.d)

# Audio files
AIFFDIR = resources/sfx/
AIFF_FILES = $(wildcard $(AIFFDIR)*.aiff)
PCMDIR = $(DFSDIR)sfx/
PCM_TMP = $(subst $(AIFFDIR),$(PCMDIR),$(AIFF_FILES))
PCM_FILES := $(PCM_TMP:.aiff=.raw)

# Sprite files
PNGDIR = resources/gfx/
PNG_FILES = $(wildcard $(PNGDIR)*.png)
SPRITEDIR = $(DFSDIR)gfx/
SPRITE_TMP = $(subst $(PNGDIR),$(SPRITEDIR),$(PNG_FILES))
SPRITE_FILES := $(SPRITE_TMP:.png=.sprite)

# N64 Emulator
MESSEMUDIR = $(N64_INST)/../mess0152-64bit
MESSEMUPATH = $(MESSEMUDIR)/mess64

# Flags
LINK_FLAGS = -L$(ROOTDIR)/lib -L$(ROOTDIR)/mips64-elf/lib
LINK_FLAGS += -ldragon -lm -lc -ldragonsys
LINK_FLAGS += -Tn64ld.x
CFLAGS = -std=gnu99
CFLAGS += -march=vr4300 -mtune=vr4300
CFLAGS += -O2 -Wall -Werror
CFLAGS += -I$(ROOTDIR)/include -I$(ROOTDIR)/mips64-elf/include
CFLAGS += -MMD -MP
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
	$(N64TOOL) \
		-b -l $(OUT_SIZE) \
		-t $(PROG_TITLE) \
		-h $(HEADERPATH)/$(HEADERNAME) \
		-o $(PROG_NAME).v64  \
		$(PROG_NAME).bin \
		-s $(DFS_OFFSET) $(PROG_NAME).dfs
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

emulate: $(PROG_NAME).v64
	sh ./run_emulator.sh $(shell pwd)/$(PROG_NAME).v64

clean:
	rm -Rf $(DFSDIR)
	rm -f *.v64 *.elf src/*.o src/*.d *.bin *.dfs

.PHONY: all clean

# Ensure object files are regenerated after header modification
-include $(DEPS)
