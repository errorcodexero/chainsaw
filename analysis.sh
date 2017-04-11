#!/bin/bash

counter=0

echo =========================================================
echo Unit Test Results
echo =================

search="process terminating"

for i in $1/*.log ;
do
    grep -i "assertion" $i | grep --quiet "failed"
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
echo =========================================================

if [ $counter -eq 0 ]; then
    echo All unit tests passed
else
    echo Unit tests failed
fi

