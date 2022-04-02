#!/bin/sh
EX=$1
rm -f $EX/*.o
gcc $EX/$EX.c -o $EX/$EX.o
chmod +x $EX/$EX.o
$EX/$EX.o $EX/$EX.in
rm -f $EX/*.o
