#!/bin/bash

server_id=1
num_entries=$2
test_memory=$3

for ((i=1; i <=10; i++))
do 
	./$1 config.txt $server_id $num_entries $test_memory
    sleep 3
done