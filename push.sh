#!/bin/bash

MSG=$*
if [ "$MSG" = "" ]
then
  MSG="-"
fi

git add --all
git commit -m "$MSG"
git push