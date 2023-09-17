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
N64_ROM_TITLE := Flappy Bird for N64
N64_ROM_FILE := $(N64_ROM_NAME).z64

all: $(N64_ROM_FILE)
.PHONY: all

# Project directories
SOURCE_DIR := ./src
RESOURCES_DIR := ./resources
BUILD_DIR := ./build
DFS_ROOTDIR := $(BUILD_DIR)/dfs

include $(N64_INST)/include/n64.mk

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
WAV64_DIR := $(DFS_ROOTDIR)/sfx
WAV_FILES := $(wildcard $(WAV_DIR)/*.wav)
WAV64_FILES := $(patsubst $(WAV_DIR)/%.wav,$(WAV64_DIR)/%.wav64,$(WAV_FILES))

# Image files
PNG_DIR := $(RESOURCES_DIR)/gfx
SPRITE_DIR := $(DFS_ROOTDIR)/gfx
PNG_FILES := $(wildcard $(PNG_DIR)/*.png)
SPRITE_FILES := $(patsubst $(PNG_DIR)/%.png,$(SPRITE_DIR)/%.sprite,$(PNG_FILES))
SPRITE_MANIFEST_TXT := $(PNG_DIR)/manifest.txt

# Build artifacts
DFS_FILE := $(BUILD_DIR)/$(N64_ROM_NAME).dfs
LINKED_OBJS := $(BUILD_DIR)/$(N64_ROM_NAME).elf

# LibDragon tools
N64_AUDIOCONV ?= $(N64_BINDIR)/audioconv64
N64_MKSPRITE ?= $(N64_BINDIR)/mksprite

#
# Compilation pipeline
#

# Final N64 ROM file in big-endian format
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

# Filesystem
# Workaround for n64.mk bug with nested filesystems
# See: https://github.com/DragonMinded/libdragon/pull/436
# The body of this rule can be removed if/when the above PR is merged
$(DFS_FILE): $(SPRITE_FILES) $(WAV64_FILES)
	@mkdir -p "$(dir $@)"
	@echo "    [DFS] $@"
	$(N64_MKDFS) "$@" "$(DFS_ROOTDIR)" $(REDIRECT_STDOUT)

#
# Housekeeping
#

clean:
	rm -Rf "$(BUILD_DIR)" *.z64
	git restore '*.z64'
.PHONY: clean

# Include compiler-generated dependency files
-include $(DEPS)
