#!/usr/bin/env sh

accepted=0
total=0

for folder in dotproduct-*; do
  total=$((total + 1))
  ../../../dahlia/fuse $folder/dot.fuse -n gemm -o $folder/dot.cpp >> /dev/null
  if [ $? -eq 0 ]; then
    # BUILDBOT=http://cerberus.cs.cornell.edu:7331 ../../_scripts/batch.py --mode estimate -p 'dahlia-vmul-dse' $folder
    accepted=$((accepted + 1))
  fi
done

echo "Total: " $total
echo "Accepted: " $accepted
