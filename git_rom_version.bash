#!/usr/bin/env bash
#
# Determine the ROM version based on git status
set -eu


STATUS="$(git status --untracked-files=no --porcelain | grep -v '.z64$')"
SHA="$(git rev-parse --short HEAD)"
TAG="$(git tag --points-at HEAD)"

if [[ -z "$STATUS" ]]; then
    [[ ! -z "$TAG" ]] && echo "$TAG" || echo "$SHA"
else
    echo "${SHA}-dirty"
fi
