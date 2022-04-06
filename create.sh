#!/bin/sh
EX=$1
rm -rf $EX
mkdir $EX
cp template.c $EX/$EX.c
touch $EX/$EX.in
touch $EX/$EX.txt
