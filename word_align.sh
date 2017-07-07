#!/bin/bash
set -eu
FILE_SIZE=$(wc -c < "$1")
ALIGN_PAD=$((4 - $FILE_SIZE % 4))
[ $ALIGN_PAD -lt 4 ] && dd if=/dev/zero bs=1 count=$ALIGN_PAD >> $1 2>/dev/null
