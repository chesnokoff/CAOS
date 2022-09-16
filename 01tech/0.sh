#!/usr/bin/env bash

extract_header() {
  answer="$(echo "$1" | sed -nE "s/.B #include <([a-zA-Z./\]+\.h)>/\1/p")"
  if [ -z "$answer" ]
  then
    return 1
  else
    echo "$answer"
    return 0
  fi
}


find_function () {
  for dir in $MANPATH
  do
    compressed_page="$dir/man3/$1.3.gz"
    uncompressed_page="$dir/man3/$1.3"

    if [ -f "$uncompressed_page" ]
    then
        extract_header "$(cat $uncompressed_page)" && return 0
    elif [ -f "$compressed_page" ]
    then
        extract_header "$(gunzip -c $compressed_page)" && return 0
    fi
  done

  return 1
}

IFS=':'
while read function
do
    find_function $function || echo "---"
done
