PROG_NAME = FlappyBird
PROG_TITLE = "FlappyBird64"

# Paths
PROJECT_DIR = $(CURDIR)
SDK_DIR = $(N64_INST)
SDK_LIB_DIR = $(SDK_DIR)/mips64-elf/lib
N64_GCC_PREFIX = $(SDK_DIR)/bin/mips64-elf-
SRC_DIR = src
DFS_DIR = filesystem

# GCC binaries
CC = $(N64_GCC_PREFIX)gcc
AS = $(N64_GCC_PREFIX)as
LD = $(N64_GCC_PREFIX)ld
OBJCOPY = $(N64_GCC_PREFIX)objcopy

# LibDragon binaries
ROM_HEADER = $(SDK_LIB_DIR)/header
LD_SCRIPT = $(SDK_LIB_DIR)/n64ld.x
CHKSUM64 = $(SDK_DIR)/bin/chksum64
MKDFS = $(SDK_DIR)/bin/mkdfs
N64TOOL = $(SDK_DIR)/bin/n64tool

# Emulator settings
MAME_DIR = /usr/local/share/mame
MAME = cd $(MAME_DIR) && mame
MAMEFLAGS = -skip_gameinfo -window -resolution 640x480

# Project files
README_TXT := README.txt
MAKEFILE := Makefile
CONVERT_GFX := convert_gfx.sh
CONVERT_SFX := convert_sfx.sh

# Code files
C_FILES := $(wildcard $(SRC_DIR)/*.c)
H_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJS := $(C_FILES:.c=.o)
DEPS := $(OBJS:.o=.d)

# GCC Flags
CFLAGS = -march=vr4300 -mtune=vr4300
CFLAGS += -std=gnu99 -O2 -Wall -Werror
CFLAGS += -I$(SDK_DIR)/include -I$(SDK_DIR)/mips64-elf/include
CFLAGS += -MMD -MP # Generate dependency files during compilation
LDFLAGS = --library=dragon --library=c --library=m --library=dragonsys
LDFLAGS += -L$(SDK_DIR)/lib -L$(SDK_LIB_DIR) --script=$(LD_SCRIPT)

# Audio files
AIFF_DIR = resources/sfx
AIFF_FILES := $(wildcard $(AIFF_DIR)/*.aiff)
PCM_DIR := $(DFS_DIR)/sfx
PCM_TMP = $(subst $(AIFF_DIR),$(PCM_DIR),$(AIFF_FILES))
PCM_FILES := $(PCM_TMP:.aiff=.raw)

# Sprite files
PNG_DIR = resources/gfx
PNG_FILES := $(wildcard $(PNG_DIR)/*.png)
SPRITE_DIR := $(DFS_DIR)/gfx
SPRITE_TMP = $(subst $(PNG_DIR),$(SPRITE_DIR),$(PNG_FILES))
SPRITE_FILES := $(SPRITE_TMP:.png=.sprite)

# LibDragon Flags
OUT_SIZE = 1052672B # 52672B HEADER + 1M (minimum) ROM
DFS_OFFSET = 256K
N64TOOLFLAGS = -l $(OUT_SIZE) -h $(ROM_HEADER) -t $(PROG_TITLE)
N64TOOLFLAGS += $(RAW_BINARY) -s $(DFS_OFFSET) $(DFS_FILE)

# Archive configuration
README_OFFSET = 620K
README_BIN = README.bin
# N64TOOLFLAGS += -s $(README_OFFSET) $(README_BIN)
ARCHIVE_OFFSET = 622K
SRC_ARCHIVE = $(PROG_NAME)-src.tar.bz
# N64TOOLFLAGS += -s $(ARCHIVE_OFFSET) $(SRC_ARCHIVE)
TARFLAGS = --exclude .DS_Store --exclude *.[do]
ARCHIVE_PATHS = README.txt Makefile *.sh resources src
ARCHIVE_FILES = $(README_TXT) $(MAKEFILE) $(CONVERT_GFX) $(CONVERT_SFX)
ARCHIVE_FILES += $(C_FILES) $(H_FILES) $(PNG_FILES) $(AIFF_FILES)

# Build products
DFS_FILE = $(PROG_NAME).dfs
ROM_FILE = $(PROG_NAME).z64
RAW_BINARY = $(PROG_NAME).bin
LINKED_OBJS = $(PROG_NAME).elf

BUILD_ARTIFACTS = $(ROM_FILE) $(RAW_BINARY) $(LINKED_OBJS) $(DFS_FILE)
BUILD_ARTIFACTS += $(OBJS) $(DEPS) $(DFS_DIR) $(SRC_ARCHIVE)

# Compilation pipeline

all: $(ROM_FILE)

# ROM Image
$(ROM_FILE): $(RAW_BINARY) $(DFS_FILE) $(README_BIN) $(SRC_ARCHIVE)
	@rm -f $@
	$(N64TOOL) -o $@ $(N64TOOLFLAGS)
	$(CHKSUM64) $@

# Raw stripped binary
$(RAW_BINARY): $(LINKED_OBJS)
	$(OBJCOPY) -O binary $< $@

# Linked object code binary
$(LINKED_OBJS): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

# Filesystem pipeline

# Sprites
$(SPRITE_FILES): $(PNG_FILES)
	sh ./convert_gfx.sh $?

# PCM Audio
$(PCM_FILES): $(AIFF_FILES)
	sh ./convert_sfx.sh $?

# DragonFS file
$(DFS_FILE): $(SPRITE_FILES) $(PCM_FILES)
	@find $(DFS_DIR) -name ".DS_Store" -depth -exec rm {} \;
	$(MKDFS) $@ $(DFS_DIR)

# README baked into ROM file
$(README_BIN): $(README_TXT)
	cp $^ $@
	# TODO Word-align README file

# Source archive
$(SRC_ARCHIVE): $(ARCHIVE_FILES)
	tar -cjf $@ $(TARFLAGS) $(ARCHIVE_PATHS)
	# TODO Word-align archive file

# Testing

# Load in MESS Emulator
emulate: $(ROM_FILE)
	$(MAME) n64 -cartridge $(PROJECT_DIR)/$< $(MAMEFLAGS)

# Everdrive64 Loader
ED64_LOADER = $(SDK_DIR)/bin/ed64-loader
everdrive: $(ROM_FILE)
	$(ED64_LOADER) -pwf $<

# Housekeeping

clean:
	rm -Rf $(BUILD_ARTIFACTS)

.PHONY: all emulate clean

# Ensure object files are regenerated after header modification
-include $(DEPS)
