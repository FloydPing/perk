#!/usr/bin/env bash

directories=(
  "perk-128-fast-3"
  "perk-128-fast-5"
  "perk-128-short-3"
  "perk-128-short-5"
  "perk-192-fast-3"
  "perk-192-fast-5"
  "perk-192-short-3"
  "perk-192-short-5"
  "perk-256-fast-3"
  "perk-256-fast-5"
  "perk-256-short-3"
  "perk-256-short-5"
)

# Iterate through the list
for dir in "${directories[@]}"; do
    if [ -d "$dir" ]; then
        cd ${dir}
        make -j8
        ./build/bin/${dir} >> ../data.txt 
        cd ..
    else
        echo "Warning: $dir is not a directory"
    fi
done
