#!/bin/bash

server_id=0
num_entries=$2
test_memory=$3
output=$1_output.txt

if [ -f $output ] ; then
    rm -f $output
fi

for ((i=1; i <=10; i++))
do
    echo $i >> $output
	./$1 config.txt $server_id $num_entries $test_memory >> $output
    sleep 1
done