#!/usr/bin/env sh

PREFIX="$1"

ls jobs/ | parallel --bar --jobs 200% "cat jobs/{}/info.json | jq -cr '[.config.hwname, .name] | @csv' | grep $PREFIX | awk -F , '{print \$2}' | sed 's/\"//g ' " > $PREFIX-jobs
