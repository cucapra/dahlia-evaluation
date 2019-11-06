#!/usr/bin/env sh

accepted=0
total=0
accept_file=data/accepted.txt

rm -f $accept_file
echo "bench" > $accept_file

for folder in ./dahlia-dot-product-*; do
  total=$((total + 1))
  ../../../dahlia/fuse $folder/dot.fuse >> /dev/null
  if [ $? -eq 0 ]; then
    accepted=$((accepted + 1))
    echo $folder | tee -a $accept_file
  fi
done

echo "Total: " $total
echo "Accepted: " $accepted
