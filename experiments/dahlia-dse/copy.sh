prefix=../../rewrite
excluded=(
  machsuite-aes
  machsuite-backprop 
  machsuite-fft-transpose
  machsuite-viterbi
)

for file in $(ls $prefix); do
  if [[ ! "${excluded[@]}" =~ "$file" ]] && [ -d "$prefix/$file" ]; then
    cp -r "$prefix/$file" .
  fi
done
