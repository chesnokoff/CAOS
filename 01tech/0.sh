#!/usr/bin/env bash

MAN3_PATH="$(find $MANPATH -type d -name "man3" | head -1)"

#echo "Found $MAN3_PATH"

while read FUNCTION
do
    #echo "$FUNCTION"
    #Check end of input
    if [ -z "$FUNCTION" ]
    then
        break
    fi 
    #Check man3 file exictanse
    FILE="$(find $MAN3_PATH -type f -name "$FUNCTION.3*")"
    #echo $FILE
    if [ -z $FILE ]
    then
        echo "---"
        continue
    fi
    TMP_FILE="./tmp"
    if [ $FILE==".*.gz" ]
    then
        #echo "unzip"
        TMP_FILE="./tmp.gz"
        cp $FILE $TMP_FILE 
        gunzip $TMP_FILE
        TMP_FILE=${TMP_FILE::-3}
    fi
    #echo $TMP_FILE

    sed -n 's/[^<]*<\([^>]*\).*/\1/p' $TMP_FILE

    #echo "delete"
    rm $TMP_FILE
done
