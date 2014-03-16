#!/bin/sh
#
# Convert pngs into 16-bit libdragon sprites

MKSPRITE=${N64_INST}/bin/mksprite

command -v ${MKSPRITE} >/dev/null 2>&1 || { \
  echo >&2 'This script requires the `mksprite` command.'; \
  echo >&2 'Make sure your N64_INST environment var is set.'; \
  exit 1; \
}

PNG_DIR="resources/gfx"
PNG_EXT=".png"
SPRITE_DIR="filesystem/gfx"
SPRITE_EXT=".sprite"
MANIFEST="${PNG_DIR}/manifest.txt"
BIT_DEPTH=16

mkdir -p ${SPRITE_DIR}

# Loop through the manifest
while read META_LINE; do
  eval META=($META_LINE)
  FILE_BASENAME=${META[0]}
  H_SLICES=${META[1]}
  V_SLICES=${META[2]}
  PNG_FILE="${PNG_DIR}/${FILE_BASENAME}${PNG_EXT}"
  SPRITE_FILE="${SPRITE_DIR}/${FILE_BASENAME}${SPRITE_EXT}"
  echo "Making sprite for $FILE_BASENAME"
  $MKSPRITE $BIT_DEPTH $H_SLICES $V_SLICES $PNG_FILE $SPRITE_FILE
done < ${MANIFEST}
