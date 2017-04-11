#!/bin/bash

counter=0

for i in $1/*.log ;
do
    grep -i "process terminating" --quiet $i
    if [ $? -eq 0 ]; then
	echo Unit test $i failed
	counter=$((counter+1))
    fi

    grep -i "definitely lost" $i | grep --quiet -v " 0 bytes"
    if [ $? -eq 0 ]; then
	echo Unit test $i leaked memory
	counter=$((counter+1))
    fi
done

if [ $counter -eq 0 ]; then
    echo All unit tests passed
else
    echo Unit tests failed
fi

