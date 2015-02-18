PROG_NAME = FlappyBird
PROG_TITLE = "FlappyBird64"

# Paths
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIR := $(notdir $(patsubst %/,%,$(dir $(MAKEFILE_PATH))))
DFS_DIR = filesystem
SDK_DIR = $(N64_INST)
ROM_HEADER = $(SDK_DIR)/lib/header
LD_SCRIPT = $(SDK_DIR)/lib/n64ld.x
N64_GCC_PREFIX = $(SDK_DIR)/bin/mips64-elf-

# GCC Binaries
CC = $(N64_GCC_PREFIX)gcc
AS = $(N64_GCC_PREFIX)as
LD = $(N64_GCC_PREFIX)ld
OBJCOPY = $(N64_GCC_PREFIX)objcopy

# LibDragon Binaries
CHKSUM64 = $(SDK_DIR)/bin/chksum64
MKDFS = $(SDK_DIR)/bin/mkdfs
N64TOOL = $(SDK_DIR)/bin/n64tool

# Code files
SRC_DIR = src
C_FILES = $(wildcard $(SRC_DIR)/*.c)
H_FILES = $(wildcard $(SRC_DIR)/*.h)
OBJS := $(C_FILES:.c=.o)
DEPS := $(OBJS:.o=.d)

# Audio files
AIFF_DIR = resources/sfx
AIFF_FILES = $(wildcard $(AIFF_DIR)/*.aiff)
PCM_DIR = $(DFS_DIR)/sfx
PCM_TMP = $(subst $(AIFF_DIR),$(PCM_DIR),$(AIFF_FILES))
PCM_FILES := $(PCM_TMP:.aiff=.raw)

# Sprite files
PNG_DIR = resources/gfx
PNG_FILES = $(wildcard $(PNG_DIR)/*.png)
SPRITE_DIR = $(DFS_DIR)/gfx
SPRITE_TMP = $(subst $(PNG_DIR),$(SPRITE_DIR),$(PNG_FILES))
SPRITE_FILES := $(SPRITE_TMP:.png=.sprite)

# GCC Flags
CFLAGS = -march=vr4300 -mtune=vr4300
CFLAGS += -std=gnu99 -O2 -Wall -Werror
CFLAGS += -I$(SDK_DIR)/include -I$(SDK_DIR)/mips64-elf/include
CFLAGS += -MMD -MP # Generate dependency files during compilation
LDFLAGS = --library=dragon --library=c --library=m --library=dragonsys
LDFLAGS += -L$(SDK_DIR)/lib -L$(SDK_DIR)/mips64-elf/lib
LDFLAGS += --script=$(LD_SCRIPT)

# LibDragon Flags
OUT_SIZE = 1052672B # 52672B HEADER + 1M (minimum) ROM
DFS_OFFSET = 256K
SRC_OFFSET = 622K
N64TOOLFLAGS = -l $(OUT_SIZE) -h $(ROM_HEADER) -t $(PROG_TITLE)
N64TOOLFLAGS += $(RAW_BINARY) -s $(DFS_OFFSET) $(DFS_FILE)

# Archive configuration
SOURCE_ARCHIVE = $(PROG_NAME)-src.tar.bz
SOURCE_PATHS = src Makefile *.sh resources
TARFLAGS = --exclude .DS_Store --exclude *.[do]

# Build products
DFS_FILE = $(PROG_NAME).dfs
ROM_FILE = $(PROG_NAME).z64
RAW_BINARY = $(PROG_NAME).bin
LINKED_OBJS = $(PROG_NAME).elf

BUILD_PRODUCTS = $(ROM_FILE) $(RAW_BINARY) $(LINKED_OBJS) $(DFS_FILE)
BUILD_PRODUCTS += $(OBJS) $(DEPS) $(DFS_DIR) $(SOURCE_ARCHIVE)

# Compilation pipeline

all: $(ROM_FILE)

# ROM Image
$(ROM_FILE): $(RAW_BINARY) $(DFS_FILE)
	@rm -f $@
	$(N64TOOL) -o $@ $(N64TOOLFLAGS)
	$(CHKSUM64) $@

# Raw stripped binary
$(RAW_BINARY): $(LINKED_OBJS)
	$(OBJCOPY) -O binary $^ $@

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

# Source archive
$(SOURCE_ARCHIVE): $(C_FILES) $(H_FILES) $(PNG_FILES) $(AIFF_FILES)
	tar -cjf $@ $(TARFLAGS) $(SOURCE_PATHS)

# Testing

# Emulator settings
MESS_DIR = $(PROJECT_DIR)/../mess0158-64bit
MESS = cd $(MESS_DIR) && ./mess64
MESSFLAGS = -skip_gameinfo -window -resolution 640x480
emulate: $(ROM_FILE)
	$(MESS) n64 -cartridge $< $(MESSFLAGS)

# Everdrive64 Loader
ED64_LOADER = $(SDK_DIR)/bin/ed64-loader
everdrive: $(ROM_FILE)
	$(ED64_LOADER) -pwf $<

# Housekeeping

clean:
	rm -Rf $(BUILD_PRODUCTS)

.PHONY: all emulate clean

# Ensure object files are regenerated after header modification
-include $(DEPS)
