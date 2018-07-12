#!/bin/bash

server_id=1
test_memory=$2

for ((i=1; i <=10; i++))
do
	./$1 config.txt $server_id $test_memory
    sleep 3
done