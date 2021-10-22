#!/usr/bin/env bash
#
# Determine the ROM version based on git status
#

set -euo pipefail

GIT_STATUS="$(\
    git status --porcelain --untracked-files=no | \
    { grep -v '.z64$' || test $? = 1; } \
)"
VERSION_TAG="$(git tag --list 'v*'--points-at HEAD)"
COMMIT_REV="git-$(git rev-parse --short HEAD)"

if [[ -z "${GIT_STATUS}" ]]; then
    if [[ -n "${VERSION_TAG}" ]]; then
        echo "${VERSION_TAG}"
    else
        echo "${COMMIT_REV}"
    fi
else
    echo "${COMMIT_REV}-dirty"
fi
