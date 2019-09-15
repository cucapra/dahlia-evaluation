#!/bin/bash

if ! hash jq 2>/dev/null; then
  echo 'Install jq from https://stedolan.github.io/jq/download/'
  exit 1
fi

aws ec2 describe-fpga-images --owners 238771226843 | jq '.FpgaImages[] | .FpgaImageId' | xargs -L 1 aws ec2 delete-fpga-image --fpga-image-id
