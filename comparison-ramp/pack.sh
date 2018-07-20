#!/bin/bash

num_entries=1
output=$1.output

if [ -f $output ] ; then
    rm -f $output
fi

for ((i=1; i <=9; i++))
do
    echo "***********************Test Start********************"
    echo $i >> $output
    echo $num_entries >> $output

    for ((k=1; k <=10; k++))
    do
	    ./$1 $((num_entries)) >> $output
        sleep 1
    done
    echo "" >> $output
    num_entries=$((num_entries*10))
    echo "***********************Test Done*************************"
done