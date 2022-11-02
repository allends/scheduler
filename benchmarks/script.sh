#!/bin/bash
if [ -f "output" ]; then
  rm output
fi

cd ..
make clean 
make all
cd ./benchmarks

COUNTER=0
while [ $COUNTER -lt 9 ]; do
  ./parallel_cal >> output
  let "COUNTER+=1"
done