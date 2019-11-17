#!/usr/bin/env sh

fuse $1 > /dev/null 2>&1

if [ $? -eq 0 ]; then
  echo $1
fi
