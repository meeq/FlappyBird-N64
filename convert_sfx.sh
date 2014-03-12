#!/bin/sh
#
# Convert aiff into raw mono 44.1Khz 16-bit signed-integer big-endian PCM data

command -v sox >/dev/null 2>&1 || { \
  echo >&2 'This script requires the `sox` command.'; \
  echo >&2 'Get it from http://sox.sourceforge.net/sox.html'; \
  exit 1; \
}

AIFF_DIR="resources/sfx"
AIFF_EXT=".aiff"
RAW_DIR="filesystem/sfx"
RAW_EXT=".raw"

mkdir -p ${RAW_DIR}

for AIFF_FILE in `ls ${AIFF_DIR}/*${AIFF_EXT}`; do
  FILE_BASENAME="$(basename -s ${AIFF_EXT} ${AIFF_FILE})"
  RAW_FILE="${RAW_DIR}/${FILE_BASENAME}${RAW_EXT}"
  sox ${AIFF_FILE} -b 16 -e signed-integer -B -r 44100 ${RAW_FILE} remix -
done
