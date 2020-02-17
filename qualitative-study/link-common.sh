#!/bin/bash

set -euf -o pipefail

FILES="fuse.mk libs sdaccel.ini utility utils.mk sda.mk"

MACHSUITE_PRE=_machsuite-common
MACHSUITE="support.cpp support.h"

# Create link to common libs
for file in $FILES; do
  echo "$file"
  ln -sf "$COMMON"/"$file" "$file"
done

# Create link to machsuite common
for file in $MACHSUITE; do
  ln -sf "$COMMON"/"$MACHSUITE_PRE"/"$file" "$file"
done
