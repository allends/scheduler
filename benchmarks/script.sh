#!/bin/bash
if [ -f "output" ]; then
  rm output
fi

COUNTER=0
while [ $COUNTER -lt 9 ]; do
  ./external_cal >> output
  let "COUNTER+=1"
done