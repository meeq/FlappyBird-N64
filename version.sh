#!/bin/bash
# Derive ROM version from Git state

# Check if we're exactly on a v* tag
tag=$(git describe --exact-match --tags --match 'v*' HEAD 2>/dev/null)

# Check if working tree is dirty
if git diff --quiet HEAD 2>/dev/null; then
    dirty=""
else
    dirty="-dirty"
fi

# If on a clean tag, use the tag name
if [[ -n "$tag" && -z "$dirty" ]]; then
    echo "$tag"
    exit 0
fi

# Otherwise use short SHA with optional -dirty suffix
sha=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
echo "Build ${sha}${dirty}"
