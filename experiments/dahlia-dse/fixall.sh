function ln_pre {
  local loc=$(file $1 | cut -d' ' -f6)
  rm $1
  ln -sfn ../$loc $1
}

function get_broken {
  local arr=( )
  for file in $(ls); do
    if [ "$(file $file | cut -d' ' -f2)" = "broken" ]; then
      echo $file
    fi
  done
}

for file in $(ls); do
  if [ -d $file ]; then
    cd $file
    for f in $(get_broken); do ln_pre $f ..; done
    cd ..
  fi
done
