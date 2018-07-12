#!/bin/bash

server_id=0
test_memory=$2
output=$1_output.txt

if [ -f $output ] ; then
    rm -f $output
fi

for ((i=1; i <=10; i++))
do
	./$1 config.txt $server_id $test_memory >> $output
    sleep 3
done