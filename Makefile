# FlappyBird-N64 - Makefile
#
# Copyright 2017-2022, Christopher Bonhage
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

#
# Variables
#

# ROM details
N64_ROM_NAME := FlappyBird
N64_ROM_FILE := $(N64_ROM_NAME).z64

all: $(N64_ROM_FILE)
.PHONY: all

# Project directories
SOURCE_DIR := ./src
RESOURCES_DIR := ./resources
BUILD_DIR := ./build
N64_MKDFS_ROOT := $(BUILD_DIR)/dfs

include $(N64_INST)/include/n64.mk

# ROM version derived from Git (can override via environment)
ROM_VERSION ?= $(shell ./version.sh)
CFLAGS += -DROM_VERSION='"$(ROM_VERSION)"'

# Set V=1 to enable verbose Make output
ifneq ($(V),1)
REDIRECT_STDOUT := >/dev/null
endif

# Code artifacts
C_FILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_FILES))
DEPS := $(OBJS:.o=.d)

# Sound files
WAV_DIR := $(RESOURCES_DIR)/sfx
WAV64_DIR := $(N64_MKDFS_ROOT)/sfx
WAV_FILES := $(wildcard $(WAV_DIR)/*.wav)
WAV64_FILES := $(patsubst $(WAV_DIR)/%.wav,$(WAV64_DIR)/%.wav64,$(WAV_FILES))

# Image files
PNG_DIR := $(RESOURCES_DIR)/gfx
SPRITE_DIR := $(N64_MKDFS_ROOT)/gfx
PNG_FILES := $(wildcard $(PNG_DIR)/*.png)
SPRITE_FILES := $(patsubst $(PNG_DIR)/%.png,$(SPRITE_DIR)/%.sprite,$(PNG_FILES))
SPRITE_MANIFEST_TXT := $(PNG_DIR)/manifest.txt

# Font files
FONT_DIR := $(RESOURCES_DIR)/fonts
FONT64_DIR := $(N64_MKDFS_ROOT)/fonts
FONT64_FILES := $(FONT64_DIR)/at01-1x.font64 $(FONT64_DIR)/at01-2x.font64

# Build artifacts
DFS_FILE := $(BUILD_DIR)/$(N64_ROM_NAME).dfs
LINKED_OBJS := $(BUILD_DIR)/$(N64_ROM_NAME).elf

#
# Compilation pipeline
#

# Final N64 ROM
$(N64_ROM_FILE): N64_ROM_TITLE := "Flappy Bird"
$(N64_ROM_FILE): N64_ROM_SAVETYPE = eeprom4k
$(N64_ROM_FILE): $(LINKED_OBJS) $(DFS_FILE)

# Linked object code binary
$(LINKED_OBJS): $(OBJS)

#
# Filesystem pipeline
#

# Graphics
$(SPRITE_DIR)/%.sprite: $(PNG_DIR)/%.png $(SPRITE_MANIFEST_TXT)
	@mkdir -p "$(dir $@)"
	@echo "    [GFX] $<"
	export MKSPRITE="$(N64_MKSPRITE)" PNG_DIR="$(PNG_DIR)" SPRITE_DIR="$(SPRITE_DIR)" && \
		bash convert_gfx.bash "$<" $(REDIRECT_STDOUT)

# Sound Effects
$(WAV64_DIR)/%.wav64: $(WAV_DIR)/%.wav
	@mkdir -p "$(dir $@)"
	@echo "    [SFX] $<"
	$(N64_AUDIOCONV) -o "$(WAV64_DIR)" "$<" $(REDIRECT_STDOUT)

# Fonts (1x for low-res, 2x for high-res)
FONT64_SIZE_1x := 16
FONT64_SIZE_2x := 32
FONT64_OUTLINE_1x := 1
FONT64_OUTLINE_2x := 2

# Disable parallel builds for font generation to prevent race conditions
.NOTPARALLEL: $(FONT64_FILES)

$(FONT64_DIR)/at01-%.font64: $(FONT_DIR)/at01.ttf
	@mkdir -p "$(dir $@)"
	@echo "    [FONT] $< ($*)"
	$(N64_MKFONT) --size $(FONT64_SIZE_$*) --outline $(FONT64_OUTLINE_$*) --range 20-7F -o "$(FONT64_DIR)" "$<" $(REDIRECT_STDOUT)
	@mv "$(FONT64_DIR)/at01.font64" "$@"

# Filesystem
$(DFS_FILE): $(SPRITE_FILES) $(WAV64_FILES) $(FONT64_FILES)

#
# Housekeeping
#

clean:
	rm -Rf "$(BUILD_DIR)" *.z64
.PHONY: clean

# Include compiler-generated dependency files
-include $(DEPS)
