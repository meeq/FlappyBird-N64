#!/bin/sh
#
# Convert pngs into 16-bit libdragon sprites

CART_FILE="$1"
MESS_DIR="../mess0152-64bit"
MESS_EMU="mess64"
MESS_FLAGS="-window -resolution 640x480 -skip_gameinfo"

command -v ${MESS_DIR}/${MESS_EMU} >/dev/null 2>&1 || { \
  echo >&2 'This script requires the mess command-line emulator.'; \
  exit 1; \
}

pushd ${MESS_DIR}

eval "./${MESS_EMU} n64 -cartridge ${CART_FILE} ${MESS_FLAGS}"

popd
