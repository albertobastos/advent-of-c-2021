#!/bin/sh
FROM=$1
TO=$2
rm -rf $TO
cp -r $FROM $TO
mv $TO/$FROM.c $TO/$TO.c
mv $TO/$FROM.in $TO/$TO.in
mv $TO/$FROM.txt $TO/$TO.txt
