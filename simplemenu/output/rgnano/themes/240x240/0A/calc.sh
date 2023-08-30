#!/bin/bash

for i in `grep -o "[0-9]*" theme.ini`; do
        conv1=$(($i * 240))
        conv2=$(($conv1 / 320))
        #conv2=`expr $i * 240 / 320`
        if [ $conv2 -le 320 ]; then
                echo "$i -> $conv2"
        fi 
done

