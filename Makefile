ROM_NAME := FlappyBird
ROM_TITLE := "FlappyBird64"
ifndef ROM_VERSION
# Derive the ROM version from the commit hash or tag
ROM_VERSION := $(shell git describe --always --abbrev=8 --dirty --match "v[0-9]*")
endif

# Directories
PROJECT_DIR := $(CURDIR)
SDK_DIR := $(N64_INST)
SDK_LIB_DIR := $(SDK_DIR)/mips64-elf/lib
N64_GCC_PREFIX := $(SDK_DIR)/bin/mips64-elf-
LIBDRAGON_DIR := libdragon
TOOLS_DIR := $(LIBDRAGON_DIR)/tools
SRC_DIR := src
RES_DIR := resources
BUILD_DIR := build
DFS_DIR := $(BUILD_DIR)/filesystem

# GCC binaries
N64_CC := $(N64_GCC_PREFIX)gcc
N64_LD := $(N64_GCC_PREFIX)ld
N64_OBJCOPY := $(N64_GCC_PREFIX)objcopy

# LibDragon tools
AUDIOCONV64 := $(TOOLS_DIR)/audioconv64/audioconv64
CHKSUM64 := $(TOOLS_DIR)/chksum64
MKDFS := $(TOOLS_DIR)/mkdfs/mkdfs
MKSPRITE := $(TOOLS_DIR)/mksprite/mksprite
N64TOOL := $(TOOLS_DIR)/n64tool

# LibDragon linkage
LIBDRAGON_LIBS := $(LIBDRAGON_DIR)/libdragon.a $(LIBDRAGON_DIR)/libdragonsys.a
ROM_HEADER := $(LIBDRAGON_DIR)/header

# Code artifacts
C_FILES := $(wildcard $(SRC_DIR)/*.c)
H_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/$(SRC_DIR)/%.o,$(C_FILES))
DEPS := $(OBJS:.o=.d)

# Audio files
WAV_DIR := $(RES_DIR)/sfx
WAV64_DIR := $(DFS_DIR)/sfx
WAV_FILES := $(wildcard $(WAV_DIR)/*.wav)
WAV64_FILES := $(patsubst $(WAV_DIR)/%.wav,$(WAV64_DIR)/%.wav64,$(WAV_FILES))

# Sprite files
PNG_DIR := $(RES_DIR)/gfx
SPRITE_DIR := $(DFS_DIR)/gfx
PNG_FILES := $(wildcard $(PNG_DIR)/*.png)
SPRITE_FILES := $(patsubst $(PNG_DIR)/%.png,$(SPRITE_DIR)/%.sprite,$(PNG_FILES))
SPRITE_MANIFEST_TXT := $(PNG_DIR)/manifest.txt

# Build products
ROM_FILE := $(if $(ROM_VERSION),$(ROM_NAME)-$(ROM_VERSION).z64,$(ROM_NAME).z64)
DFS_FILE := $(BUILD_DIR)/$(ROM_NAME).dfs
RAW_BINARY := $(BUILD_DIR)/$(ROM_NAME).bin
LINKED_OBJS := $(BUILD_DIR)/$(ROM_NAME).elf

# Compiler flags
CFLAGS += -MMD -MP # Generate dependency files during compilation
CFLAGS += -DN64 -std=gnu99 -march=vr4300 -mtune=vr4300 -O2
CFLAGS += -Wall -Werror -Wa,--fatal-warnings -fdiagnostics-color=always
CFLAGS += -falign-functions=32 -ffunction-sections -fdata-sections
CFLAGS += -I$(SDK_DIR)/mips64-elf/include -I$(LIBDRAGON_DIR)/include
CFLAGS += -DROM_VERSION='"$(ROM_VERSION)"'

# Linker flags
LDFLAGS += --library=dragon --library=c --library=m --library=dragonsys
LDFLAGS += --library-path=$(SDK_LIB_DIR) --library-path=$(LIBDRAGON_DIR)
LDFLAGS += --script=$(LIBDRAGON_DIR)/n64.ld --gc-sections

# Compilation pipeline
all: $(ROM_FILE)
.PHONY: all

# Final N64 ROM file in big-endian format
$(ROM_FILE): $(RAW_BINARY) $(DFS_FILE) $(N64TOOL) $(CHKSUM64)
	@mkdir -p $(dir $@)
	@echo "    [Z64] $@"
	@rm -f $@
	$(N64TOOL) -o $@ --header $(ROM_HEADER) --title $(ROM_TITLE) \
		$(RAW_BINARY) \
		--offset 1M $(DFS_FILE)
	$(CHKSUM64) $@ $(REDIRECT_STDOUT)

# Raw stripped binary
$(RAW_BINARY): $(LINKED_OBJS)
	@mkdir -p $(dir $@)
	@echo "    [OBJCOPY] $(notdir $@)"
	$(N64_OBJCOPY) -O binary $< $@

# Linked object code binary
$(LINKED_OBJS): $(OBJS) $(LIBDRAGON_LIBS)
	@mkdir -p $(dir $@)
	@echo "    [LD] $(notdir $@)"
	$(N64_LD) -o $@ $^ $(LDFLAGS)

# Compiled C objects
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c 
	@mkdir -p $(dir $@)
	@echo "    [CC] $<"
	$(N64_CC) -c $(CFLAGS) -o $@ $<

# Filesystem pipeline

# Converted graphics
$(SPRITE_DIR)/%.sprite: export MKSPRITE
$(SPRITE_DIR)/%.sprite: export PNG_DIR
$(SPRITE_DIR)/%.sprite: export SPRITE_DIR
$(SPRITE_DIR)/%.sprite: $(PNG_DIR)/%.png $(SPRITE_MANIFEST_TXT) $(MKSPRITE)
	@mkdir -p $(dir $@)
	@echo "    [GFX] $<"
	bash convert_gfx.sh $<

# Converted audio
$(WAV64_DIR)/%.wav64: $(WAV_DIR)/%.wav $(AUDIOCONV64)
	@mkdir -p $(dir $@)
	@echo "    [SFX] $<"
	$(AUDIOCONV64) -o $(WAV64_DIR) $<

# Converted filesystem
$(DFS_FILE): $(SPRITE_FILES) $(WAV64_FILES) $(MKDFS)
	@mkdir -p $(dir $@)
	@echo "    [DFS] $(notdir $@)"
	@find $(DFS_DIR) -depth -name ".DS_Store" -delete
	$(MKDFS) $@ $(DFS_DIR) $(REDIRECT_STDOUT)

# LibDragon submodule

$(LIBDRAGON_LIBS): libdragon ;

$(AUDIOCONV64) $(CHKSUM64) $(MKDFS) $(MKSPRITE) $(N64TOOL): libdragon-tools ;

libdragon: gitmodules
	@echo "    [MAKE] libdragon"
	$(MAKE) -C libdragon $(REDIRECT_STDOUT)
.PHONY: libdragon

libdragon-tools: gitmodules
	@echo "    [MAKE] libdragon-tools"
	$(MAKE) -C libdragon tools $(REDIRECT_STDOUT)
.PHONY: libdragon-tools

libdragon-clean:
	$(MAKE) -C libdragon clean tools-clean $(REDIRECT_STDOUT)
.PHONY: libdragon-clean

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
	rm -Rf $(BUILD_DIR) $(ROM_NAME)-v*-*.z64 $(ROM_NAME).z64
.PHONY: clean

# Ensure submodules are up-to-date; set GITMODULES=0 to skip.
GITMODULES ?= 1
ifeq ($(GITMODULES),1)
gitmodules:
	@if git submodule status | egrep -q '^[-]|^[+]' ; then \
		echo "    [GIT] submodules"; \
		git submodule update --init; \
		$(MAKE) libdragon-clean; \
    fi
else
gitmodules: ;
endif
.PHONY: gitmodules

# Ensure object files are regenerated after header modification
-include $(DEPS)

# Silence Make sub-command directory traversal messages 
export GNUMAKEFLAGS=--no-print-directory

# Silent by default; set V=1 to enable verbose Make output
ifneq ($(V),1)
.SILENT:
REDIRECT_STDOUT := >/dev/null
else
REDIRECT_STDOUT :=
endif
