PROG_NAME = FlappyBird
PROG_TITLE = "FlappyBird64"

# Paths
PROJECT_DIR = $(CURDIR)
SDK_DIR = $(N64_INST)
SDK_LIB_DIR = $(SDK_DIR)/mips64-elf/lib
N64_GCC_PREFIX = $(SDK_DIR)/bin/mips64-elf-
SRC_DIR = src
RES_DIR = resources
DFS_DIR = filesystem

# GCC binaries
CC = $(N64_GCC_PREFIX)gcc
AS = $(N64_GCC_PREFIX)as
LD = $(N64_GCC_PREFIX)ld
OBJCOPY = $(N64_GCC_PREFIX)objcopy

# LibDragon binaries
ROM_HEADER = $(SDK_LIB_DIR)/header
CHKSUM64 = $(SDK_DIR)/bin/chksum64
MKDFS = $(SDK_DIR)/bin/mkdfs
N64TOOL = $(SDK_DIR)/bin/n64tool

# Testing settings
CEN64_DIR = $(N64_INST)/../cen64
CEN64 = $(CEN64_DIR)/cen64
CEN64FLAGS = $(CEN64_DIR)/pifdata.bin
MAME_DIR = $(N64_INST)/../mame
MAME = cd $(MAME_DIR) && ./mame64
MAMEFLAGS = -skip_gameinfo -window -resolution 640x480
ED64_LOADER = $(SDK_DIR)/bin/ed64-loader

# Project files
README_TXT := README.md
LICENSE_TXT := LICENSE
SCREENSHOTS_PNG := Screenshots.png
MAKEFILE := Makefile
CONVERT_GFX := convert_gfx.sh
WORD_ALIGN := word_align.sh

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
LDFLAGS += --library-path=$(SDK_DIR)/lib --library-path=$(SDK_LIB_DIR)
LDFLAGS += --script=n64.ld --gc-sections

# Audio files
SOX = sox
SOXFLAGS = -b 16 -e signed-integer -B -r 44100
AIFF_DIR = $(RES_DIR)/sfx
AIFF_FILES := $(wildcard $(AIFF_DIR)/*.aiff)
PCM_DIR := $(DFS_DIR)/sfx
PCM_TMP = $(subst $(AIFF_DIR),$(PCM_DIR),$(AIFF_FILES))
PCM_FILES := $(PCM_TMP:.aiff=.raw)

# Sprite files
PNG_DIR = $(RES_DIR)/gfx
PNG_FILES := $(wildcard $(PNG_DIR)/*.png)
SPRITE_MANIFEST_TXT := $(PNG_DIR)/manifest.txt
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
N64TOOLFLAGS += -s $(README_OFFSET) $(README_BIN)
ARCHIVE_OFFSET = 622K
SRC_ARCHIVE = $(PROG_NAME)-src.tar.bz
N64TOOLFLAGS += -s $(ARCHIVE_OFFSET) $(SRC_ARCHIVE)
TARFLAGS = --exclude .DS_Store --exclude *.[do]
ARCHIVE_ROOT_FILES = $(README_TXT) $(LICENSE_TXT) $(SCREENSHOTS_PNG) \
										 $(MAKEFILE) $(CONVERT_GFX) $(WORD_ALIGN)
ARCHIVE_FILES = $(ARCHIVE_ROOT_FILES) $(C_FILES) $(H_FILES) \
																			$(PNG_FILES) $(AIFF_FILES)
ARCHIVE_PATHS = $(ARCHIVE_ROOT_FILES) $(SRC_DIR) $(RES_DIR)

# Testing products
CLONE_DIR = clone
CLONE_SKIP_OFFSET = 641024

# Build products
DFS_FILE = $(PROG_NAME).dfs
ROM_FILE = $(PROG_NAME).z64
RAW_BINARY = $(PROG_NAME).bin
LINKED_OBJS = $(PROG_NAME).elf

BUILD_ARTIFACTS = $(ROM_FILE) $(RAW_BINARY) $(LINKED_OBJS)
BUILD_ARTIFACTS += $(OBJS) $(DEPS) $(DFS_FILE) $(DFS_DIR)
BUILD_ARTIFACTS += $(README_BIN) $(SRC_ARCHIVE) $(CLONE_DIR)

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
$(SPRITE_DIR)/%.sprite: $(PNG_DIR)/%.png $(SPRITE_MANIFEST_TXT)
	@bash $(CONVERT_GFX) $<

# PCM Audio
$(PCM_DIR)/%.raw: $(AIFF_DIR)/%.aiff
	@mkdir -p $(PCM_DIR)
	@command -v $(SOX) >/dev/null 2>&1 || { \
	    echo >&2 'This Makefile requires the `sox` command.'; \
	    echo >&2 'Get it from http://sox.sourceforge.net/sox.html'; \
	    exit 1; \
	}
	$(SOX) $< $(SOXFLAGS) $@ remix -

# DragonFS file
$(DFS_FILE): $(SPRITE_FILES) $(PCM_FILES)
	@find $(DFS_DIR) -depth -name ".DS_Store" -exec rm {} \;
	$(MKDFS) $@ $(DFS_DIR)

# README baked into ROM file
$(README_BIN): $(README_TXT)
	@cp $^ $@
	@bash $(WORD_ALIGN) $@


# Source archive
$(SRC_ARCHIVE): $(ARCHIVE_FILES)
	tar -cjf $@ $(TARFLAGS) $(ARCHIVE_PATHS)
	@bash $(WORD_ALIGN) $@

# Testing

# Load ROM in cen64 emulator
emulate-cen64: $(ROM_FILE)
	$(CEN64) $(CEN64FLAGS) $(PROJECT_DIR)/$<

# Load ROM in MAME emulator
emulate-mame: $(ROM_FILE)
	$(MAME) n64 -cartridge $(PROJECT_DIR)/$< $(MAMEFLAGS)

# Load ROM over USB to Everdrive64
everdrive: $(ROM_FILE)
	$(ED64_LOADER) -pwf $<

# Ensure that it is possible to build from source with the packaged ROM
test-clone: $(ROM_FILE)
	mkdir $(CLONE_DIR) && \
	cd $(CLONE_DIR) && \
	dd bs=1 skip=$(CLONE_SKIP_OFFSET) \
	   if=$(PROJECT_DIR)/$(ROM_FILE) of=source.tar.bz && \
	tar -xf source.tar.bz && \
	make

# Housekeeping

clean:
	rm -Rf $(BUILD_ARTIFACTS)

.PHONY: all emulate-cen64 emulate-mame test-clone clean

# Ensure object files are regenerated after header modification
-include $(DEPS)
