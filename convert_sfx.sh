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

convert_to_raw_pcm() {
    local AIFF_FILE=$1
    FILE_BASENAME="$(basename -s ${AIFF_EXT} ${AIFF_FILE})"
    RAW_FILE="${RAW_DIR}/${FILE_BASENAME}${RAW_EXT}"
    echo "Extracting raw PCM data from '${FILE_BASENAME}${AIFF_EXT}'"
    sox ${AIFF_FILE} -b 16 -e signed-integer -B -r 44100 ${RAW_FILE} remix -
}

mkdir -p ${RAW_DIR}

if [ $# -eq 0 ]; then
    # Convert all audio files
    for FILE in `ls ${AIFF_DIR}/*${AIFF_EXT}`; do
        convert_to_raw_pcm $FILE
    done
else
    # Convert command-line arguments
    for FILE in "$@"; do
        convert_to_raw_pcm $FILE
    done
fi
