#!/usr/bin/env bash

check_and_unzip () {
    if [ $1==".*.gz" ]
    then
        TMP_FILE="./tmp.gz"
        cp $FILE $TMP_FILE 
        gunzip $TMP_FILE
        TMP_FILE=${TMP_FILE::-3}
    else
        TMP_FILE="./tmp"
        cp $FILE $TMP_FILE
    fi
    echo "$TMP_FILE"
}

find_function () {
   for FILE in $1/*
    do
        if [ -d $FILE  ]
        then
            continue
        fi
        TMP_FILE="$(check_and_unzip $FILE)"
        LINE="$(grep "\"FILE \*$2" $TMP_FILE | head -1)"
        if [ ! -z "$LINE" ]
        then
            break
        else
            rm $TMP_FILE
        fi
    done 
    if [ ! -z "$LINE" ]
    then
        echo "$TMP_FILE"
    else
        echo ""
    fi
}

MAN3_PATH="$(find $MANPATH -type d -name "man3" | head -1)"

while read FUNCTION
do
    if [ -z "$FUNCTION" ]
    then
        break
    fi 

    TMP_FILE="$(find_function $MAN3_PATH $FUNCTION)"
    cat $TMP_FILE
    if [ -z $TMP_FILE ]
    then
        echo "---"
        continue
    fi
    sed -n 's/[^<]*<\([^>]*\).*/\1/p' $TMP_FILE

    rm $TMP_FILE
done
