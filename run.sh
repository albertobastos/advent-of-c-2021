#!/bin/sh
EX=$1
rm -f $EX/*.o
gcc $EX/main.c -o $EX/main.o
chmod +x $EX/main.o
$EX/main.o $EX/input.in
rm -f $EX/*.o
