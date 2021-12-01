# FlappyBird-N64 - Makefile
#
# Copyright 2021, Christopher Bonhage
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

#
# Variables
#

# Project directories
SRC_DIR := ./src
RES_DIR := ./resources
BUILD_DIR := ./build
OBJS_DIR := $(BUILD_DIR)/src
DFS_DIR := $(BUILD_DIR)/dfs
LIBDRAGON_DIR := ./libdragon

# ROM details
ROM_NAME ?= FlappyBird
ROM_TITLE ?= Flappy Bird for N64

# Allow ROM_VERSION to be specified; default to current git branch/tag/commit
ifndef ROM_VERSION
ROM_VERSION := $(shell bash git_rom_version.bash)
endif
# Use a "guard" file to ensure the ROM re-builds when the version changes
ROM_VERSION_GUARD := .guard-ROM_VERSION-$(ROM_VERSION)
# Expose ROM_VERSION as a string constant to the compiler
CFLAGS += -DROM_VERSION='"$(ROM_VERSION)"'

# Derive the final ROM_FILE name based on the ROM_VERSION
ROM_FILE := $(ROM_NAME).z64
ifeq ($(ROM_VERSION),)
$(error ROM_VERSION is not set. See `git_rom_version.bash`)
else
ifeq ($(shell echo "$(ROM_VERSION)" | egrep -e "-dirty$$"),)
# Include ROM_VERSION in ROM_FILE if the version is clean 
ROM_FILE := $(ROM_NAME)-$(ROM_VERSION).z64
else
# Otherwise, tag ROM_FILE as "dirty" to discourage distribution
ROM_FILE := $(ROM_NAME)-dirty.z64
endif
endif

# Set N64_INST to skip the really long toolchain build process.
# This is also useful if you are using the libdragon Docker container.
ifeq ($(N64_INST),)
TOOLCHAIN_DIR := ./toolchain
# Use a guard to cache the toolchain in-project and export it for libdragon
TOOLCHAIN_GUARD := $(TOOLCHAIN_DIR)/.installed
override N64_INST := $(realpath $(TOOLCHAIN_DIR))
endif

# Set V=1 to enable verbose Make output
ifneq ($(V),1)
.SILENT:
REDIRECT_STDOUT := >/dev/null
endif

# Set GITMODULES=0 to skip git submodule updates
GITMODULES ?= 1

# Toolchain directories
N64_INCLUDE_DIR := $(N64_INST)/mips64-elf/include
N64_LIB_DIR := $(N64_INST)/mips64-elf/lib
N64_GCC_PREFIX := $(N64_INST)/bin/mips64-elf-

# Toolchain binaries
N64_CC := $(N64_GCC_PREFIX)gcc
N64_LD := $(N64_GCC_PREFIX)ld
N64_OBJCOPY := $(N64_GCC_PREFIX)objcopy

# LibDragon tools
TOOLS_DIR := $(LIBDRAGON_DIR)/tools
AUDIOCONV64 := $(TOOLS_DIR)/audioconv64/audioconv64
CHKSUM64 := $(TOOLS_DIR)/chksum64
MKDFS := $(TOOLS_DIR)/mkdfs/mkdfs
MKSPRITE := $(TOOLS_DIR)/mksprite/mksprite
N64TOOL := $(TOOLS_DIR)/n64tool

# LibDragon artifacts
LIBDRAGON_LIBS += $(LIBDRAGON_DIR)/libdragon.a
LIBDRAGON_LIBS += $(LIBDRAGON_DIR)/libdragonsys.a
ROM_HEADER := $(LIBDRAGON_DIR)/header

# Code artifacts
C_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJS_DIR)/%.o,$(C_FILES))
DEPS := $(OBJS:.o=.d)

# Sound files
WAV_DIR := $(RES_DIR)/sfx
WAV64_DIR := $(DFS_DIR)/sfx
WAV_FILES := $(wildcard $(WAV_DIR)/*.wav)
WAV64_FILES := $(patsubst $(WAV_DIR)/%.wav,$(WAV64_DIR)/%.wav64,$(WAV_FILES))

# Image files
PNG_DIR := $(RES_DIR)/gfx
SPRITE_DIR := $(DFS_DIR)/gfx
PNG_FILES := $(wildcard $(PNG_DIR)/*.png)
SPRITE_FILES := $(patsubst $(PNG_DIR)/%.png,$(SPRITE_DIR)/%.sprite,$(PNG_FILES))
SPRITE_MANIFEST_TXT := $(PNG_DIR)/manifest.txt

# Build artifacts
DFS_FILE := $(BUILD_DIR)/$(ROM_NAME).dfs
RAW_BINARY := $(BUILD_DIR)/$(ROM_NAME).bin
LINKED_OBJS := $(BUILD_DIR)/$(ROM_NAME).elf

# Compiler flags
CFLAGS += -MMD -MP # Generate dependency files during compilation
CFLAGS += -DN64 -march=vr4300 -mtune=vr4300 -std=gnu99 -Og -ggdb3
CFLAGS += -Wall -Werror -Wa,--fatal-warnings -fdiagnostics-color=always
CFLAGS += -falign-functions=32 -ffunction-sections -fdata-sections
CFLAGS += -I"$(LIBDRAGON_DIR)/include" -I"$(N64_INCLUDE_DIR)"

# Linker flags
LDFLAGS += --library-path="$(LIBDRAGON_DIR)" --library-path="$(N64_LIB_DIR)"
LDFLAGS += --library=dragon --library=c --library=m --library=dragonsys
LDFLAGS += --script="$(LIBDRAGON_DIR)/n64.ld" --gc-sections

#
# Compilation pipeline
#

all: toolchain libdragon libdragon-tools $(ROM_FILE)
.PHONY: all

# Final N64 ROM file in big-endian format
$(ROM_FILE): $(ROM_VERSION_GUARD)
$(ROM_FILE): $(RAW_BINARY) $(DFS_FILE)
$(ROM_FILE): $(N64TOOL) $(CHKSUM64)
	@mkdir -p "$(dir $@)"
	@echo "    [Z64] $@"
	@rm -f "$@"
	$(N64TOOL) -o "$@" --header "$(ROM_HEADER)" --title "$(ROM_TITLE)" \
		"$(RAW_BINARY)" \
		--offset 1M "$(DFS_FILE)"
	$(CHKSUM64) "$@" $(REDIRECT_STDOUT)

# Raw stripped binary
$(RAW_BINARY): $(LINKED_OBJS)
	@mkdir -p "$(dir $@)"
	@echo "    [BIN] $(notdir $@)"
	$(N64_OBJCOPY) -O binary "$<" "$@"

# Linked object code binary
$(LINKED_OBJS): $(OBJS) $(LIBDRAGON_LIBS)
	@mkdir -p "$(dir $@)"
	@echo "    [LD]  $(notdir $@)"
	$(N64_LD) -o "$@" $^ $(LDFLAGS)

# Compiled C objects
$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c $(TOOLCHAIN_GUARD)
	@mkdir -p "$(dir $@)"
	@echo "    [CC]  $<"
	$(N64_CC) -c $(CFLAGS) -o "$@" "$<"

#
# Filesystem pipeline
#

# Graphics
$(SPRITE_DIR)/%.sprite: $(PNG_DIR)/%.png $(SPRITE_MANIFEST_TXT) $(MKSPRITE)
	@mkdir -p "$(dir $@)"
	@echo "    [GFX] $<"
	export MKSPRITE="$(MKSPRITE)" PNG_DIR="$(PNG_DIR)" SPRITE_DIR="$(SPRITE_DIR)" && \
		bash convert_gfx.bash "$<" $(REDIRECT_STDOUT)

# Sound Effects
$(WAV64_DIR)/%.wav64: $(WAV_DIR)/%.wav $(AUDIOCONV64)
	@mkdir -p "$(dir $@)"
	@echo "    [SFX] $<"
	$(AUDIOCONV64) -o "$(WAV64_DIR)" "$<" $(REDIRECT_STDOUT)

# Filesystem
$(DFS_FILE): $(SPRITE_FILES) $(WAV64_FILES) $(MKDFS)
	@mkdir -p "$(dir $@)"
	@echo "    [DFS] $(notdir $@)"
	@find "$(DFS_DIR)" -depth -name ".DS_Store" -delete
	$(MKDFS) "$@" "$(DFS_DIR)" $(REDIRECT_STDOUT)

#
# LibDragon submodule
#

$(LIBDRAGON_LIBS): | libdragon ;

$(AUDIOCONV64) $(CHKSUM64) $(MKDFS) $(MKSPRITE) $(N64TOOL): | libdragon-tools ;

libdragon: | gitmodules
	@echo "    [DEP] $@"
	$(MAKE) -C "$(LIBDRAGON_DIR)" libdragon \
		V=$(V) N64_INST="$(N64_INST)" D=1 $(REDIRECT_STDOUT)
.PHONY: libdragon

libdragon-tools: | gitmodules
	@echo "    [DEP] $@"
	$(MAKE) -C "$(LIBDRAGON_DIR)" tools $(REDIRECT_STDOUT)
.PHONY: libdragon-tools

libdragon-clean:
	$(MAKE) -C "$(LIBDRAGON_DIR)" clean tools-clean $(REDIRECT_STDOUT)
.PHONY: libdragon-clean

#
# Compiler toolchain
#

ifdef TOOLCHAIN_GUARD
# Track the toolchain installation with a guard file
toolchain: $(TOOLCHAIN_GUARD)

# Copy the toolchain script from libdragon and run it
$(TOOLCHAIN_GUARD): | gitmodules
	@mkdir -p "$(TOOLCHAIN_DIR)/tmp"
	@echo "    [DEP] toolchain (this may take a while!)"
	cp "$(TOOLS_DIR)/build-toolchain.sh" "$(TOOLCHAIN_DIR)/tmp"
	export N64_INST="$(N64_INST)" && \
		cd "$(TOOLCHAIN_DIR)/tmp" && \
		bash build-toolchain.sh
	rm -Rf "$(TOOLCHAIN_DIR)/tmp"
	touch "$@"

toolchain-clean:
	rm -Rf "$(TOOLCHAIN_DIR)"
else
# NO-OP: toolchain is managed externally from the project
toolchain toolchain-clean: ;
endif
.PHONY: toolchain toolchain-clean

#
# Testing
#

# Load ROM in cen64 emulator
ifdef CEN64_DIR
CEN64 = $(CEN64_DIR)/cen64

emulate-cen64: $(ROM_FILE)
	$(CEN64) $(CEN64FLAGS) "$(CEN64_DIR)/pifdata.bin" "$(CURDIR)/$<"
.PHONY: emulate-cen64
endif

# Load ROM in MAME emulator
ifdef MAME_DIR
MAME = cd "$(MAME_DIR)" && ./mame64
MAMEFLAGS = -skip_gameinfo -window -resolution 640x480

emulate-mame: $(ROM_FILE)
	$(MAME) n64 -cartridge "$(CURDIR)/$<" $(MAMEFLAGS)
.PHONY: emulate-mame
endif

#
# Housekeeping
#

clean:
	rm -Rf "$(BUILD_DIR)" "$(ROM_NAME)-dirty.z64"
.PHONY: clean

distclean:
	rm -Rf "$(BUILD_DIR)" "$(LIBDRAGON_DIR)" *.z64 .guard-*
	git restore "$(LIBDRAGON_DIR)" '*.z64'
.PHONY: distclean

# Rebuild files that depend on the ROM_VERSION when it changes
$(ROM_VERSION_GUARD):
	rm -f "$(OBJS_DIR)/ui.o"
	rm -f .guard-ROM_VERSION-* 
	touch "$@"

ifeq ($(GITMODULES),1)
# Update git submodules
gitmodules:
	@if git submodule status | egrep -q '^[-]|^[+]' ; then \
		echo "    [GIT] submodules"; \
		git submodule update --init $(REDIRECT_STDOUT); \
		$(MAKE) libdragon-clean V=$(V) $(REDIRECT_STDOUT); \
    fi
else
gitmodules: ;
endif
.PHONY: gitmodules

# Include compiler-generated dependency files
-include $(DEPS)
