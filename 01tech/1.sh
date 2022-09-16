#!/usr/bin/env bash

declare -A matrix
default_IFS=$IFS
accuracy=40
size=0 #Number of raws
raw=0
column=0
epsilon=`echo "scale=$accuracy; 1 / 10^10" | bc -l`

abs () {
  if (( $(echo "$1 < 0" | bc -l) )); then
    echo "$1*-1" | bc -l
  else
    echo "$1"
  fi
}

get_matrix () {
  #$1 - path to csv file
  while IFS=$default_IFS read line; do
    IFS=','
    for element in $line; do
      matrix[$raw,$column]="$element" 
      ((column+=1))
    done
    column=0
    ((raw+=1))
  done < $1
  size=$raw
}

multiply () {
  #Multiply raw numer $1 by $2
  for ((i=0;i<size+1;i++)) do
    matrix[$1,$i]=`echo "scale=$accuracy; ${matrix[$1,$i]} * $2" | bc -l`
  done 
}

divide () {
  #Divide raw number $1 by $2
  for ((i=0;i<size+1;i++)) do
    matrix[$1,$i]=`echo "scale=$accuracy; ${matrix[$1,$i]} / $2" | bc -l`
  done 
}

add () {
  #Add raw $1 to $2
  for ((i=0;i<size+1;i++)) do
    matrix[$2,$i]=`echo "scale=$accuracy; ${matrix[$1,$i]} + ${matrix[$2,$i]}" | bc -l`
  done  
}

substruct () {
  #Substruct raw $1 from $2
  for ((i=0;i<size+1;i++)) do
    matrix[$2,$i]=`echo "scale=$accuracy; ${matrix[$1,$i]} - ${matrix[$2,$i]}" | bc -l`
  done  
}

change () {
  #Change raws $1 and $2
  for ((i=0;i<size+1;i++)) do
    tmp=${matrix[$1,$i]}
    matrix[$1,$i]=${matrix[$2,$i]}
    matrix[$2,$i]=$tmp
  done
}

find_pivot_raw () {
  #Find pivot's raw number for column $1
  max="$(abs ${matrix[0,$1]})"
  answer=0
  for ((j=1;j<size;++j)) do
    if (( $(echo "$(abs ${matrix[$j,$1]}) > $max" | bc -l) )); then
      answer=$j
      max="$(abs ${matrix[$j,$1]})"
    fi
  done
  echo "$answer"
}

tiny_check_raw () {
  #Check raw $1 for small numbers and change them to 0
  for ((i=0;i<size+1;i++)) do
    if (( $(echo "$(abs ${matrix[$1,$i]}) < $epsilon" | bc -l) )); then
      matrix[$1,$i]=0
    fi
  done 
}

tiny_check_column () {
  #Check column $1 for small numbers and change them to 0
  for ((i=0;i<size;i++)) do
    if (( $(echo "$(abs ${matrix[$i,$1]}) < $epsilon" | bc -l) )); then
      matrix[$i,$1]=0
    fi
  done 
}

print_matrix () {
  echo "Start printing"
    for ((i=0;i<size;i++)) do
        line=""
        for ((j=0;j<size+1;j++)) do
          line+="${matrix[$i,$j]} "
        done
        echo "$line"
    done
}

make_triangle () {
  for((raw=0;raw<size;raw++)) do
    tiny_check_raw $raw
    tiny_check_column $raw
    if (( $(echo "${matrix[$raw,$raw]} == 0" | bc -l) )); then
      change $raw "$(find_pivot_raw $raw)"
    fi
    divide $raw ${matrix[$raw,$raw]}
    for ((extra_raw=raw+1;extra_raw<size;extra_raw++)) do
        if (( $(echo "${matrix[$extra_raw,$raw]} != 0" | bc -l) )); then
          divide $extra_raw ${matrix[$extra_raw,$raw]}
          tiny_check_raw $extra_raw
          divide $extra_raw ${matrix[$extra_raw,$raw]}
          substruct $raw $extra_raw
          tiny_check_raw $extra_raw
        fi
    done
  done
}

reverse_word_order() {
  local result=
  for word in $@; do
    result="$word $result"
  done
  echo "$result" 
}

print_answer () {
  IFS=' '
  line_answer="${matrix[$(($size-1)),$size]}"
  for ((i=size-2;i>=0;i--)) do
    j=$(($size-1))
    for solution in $line_answer
    do
      matrix[$i,$size]=`echo "scale=$accuracy; ${matrix[$i,$size]} - (${matrix[$i,$j]} * $solution)" | bc -l`
      j=$(($j-1))
    done
  line_answer+=" ${matrix[$i,$(($size))]}"
  done
  for answer in $(reverse_word_order $line_answer)
  do
    echo "$answer"
  done
}

get_matrix $1
make_triangle
print_answer
