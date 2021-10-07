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

# Project scripts
CONVERT_GFX := convert_gfx.sh

# Code files
C_FILES := $(wildcard $(SRC_DIR)/*.c)
H_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJS := $(C_FILES:.c=.o)
DEPS := $(OBJS:.o=.d)

# GCC Flags
CFLAGS = -march=vr4300 -mtune=vr4300
CFLAGS += -std=gnu99 -O2 -Wall -Werror
CFLAGS += -I$(SDK_DIR)/mips64-elf/include
CFLAGS += -MMD -MP # Generate dependency files during compilation
LDFLAGS = --library=dragon --library=c --library=m --library=dragonsys
LDFLAGS += --library-path=$(SDK_LIB_DIR)
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
N64TOOLFLAGS = --header $(ROM_HEADER) --title $(PROG_TITLE)

# Build products
ROM_FILE = $(PROG_NAME).z64
DFS_FILE = $(PROG_NAME).dfs
RAW_BINARY = $(PROG_NAME).bin
LINKED_OBJS = $(PROG_NAME).elf

BUILD_ARTIFACTS = $(ROM_FILE) $(RAW_BINARY) $(LINKED_OBJS)
BUILD_ARTIFACTS += $(OBJS) $(DEPS) $(DFS_FILE) $(DFS_DIR)

# Compilation pipeline

# ROM Image
$(ROM_FILE): $(RAW_BINARY) $(DFS_FILE)
	@rm -f $@
	$(N64TOOL) -o $@ $(N64TOOLFLAGS) $(RAW_BINARY) --offset 1M $(DFS_FILE)
	$(CHKSUM64) $@

# Raw stripped binary
$(RAW_BINARY): $(LINKED_OBJS)
	$(OBJCOPY) -O binary $< $@

# Linked object code binary
$(LINKED_OBJS): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

# Filesystem pipeline

# Converted graphics
$(SPRITE_DIR)/%.sprite: $(PNG_DIR)/%.png $(SPRITE_MANIFEST_TXT)
	@bash $(CONVERT_GFX) $<

# Converted audio
$(PCM_DIR)/%.raw: $(AIFF_DIR)/%.aiff
	@mkdir -p $(PCM_DIR)
	@command -v $(SOX) >/dev/null 2>&1 || { \
	    echo >&2 'This Makefile requires the `sox` command.'; \
	    echo >&2 'Get it from http://sox.sourceforge.net/sox.html'; \
	    exit 1; \
	}
	$(SOX) $< $(SOXFLAGS) $@ remix -

# Converted filesystem
$(DFS_FILE): $(SPRITE_FILES) $(PCM_FILES)
	@find $(DFS_DIR) -depth -name ".DS_Store" -exec rm {} \;
	$(MKDFS) $@ $(DFS_DIR)

# Testing

# Load ROM in cen64 emulator
ifdef CEN64_DIR
CEN64 = $(CEN64_DIR)/cen64
CEN64FLAGS = $(CEN64_DIR)/pifdata.bin

emulate-cen64: $(ROM_FILE)
	$(CEN64) $(CEN64FLAGS) $(PROJECT_DIR)/$<
.PHONY: emulate-cen64
endif

# Load ROM in MAME emulator
ifdef MAME_DIR
MAME = cd $(MAME_DIR) && ./mame64
MAMEFLAGS = -skip_gameinfo -window -resolution 640x480

emulate-mame: $(ROM_FILE)
	$(MAME) n64 -cartridge $(PROJECT_DIR)/$< $(MAMEFLAGS)
.PHONY: emulate-mame
endif

# Housekeeping

clean:
	rm -Rf $(BUILD_ARTIFACTS)
.PHONY: clean

# Ensure object files are regenerated after header modification
-include $(DEPS)
