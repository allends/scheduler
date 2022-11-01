#!/bin/bash

make clean && make all && cd benchmarks && make clean && make external_cal && cd ../ && ./benchmarks/genRecord.sh ./benchmarks/external_cal

#make clean && make all && cd benchmarks && make clean && make vector_multiply && cd ../ && ./benchmarks/vector_multiply
#make clean && make all && cd benchmarks && make clean && make parallel_cal && cd ../ && ./benchmarks/parallel_cal
#make clean && make all && cd benchmarks && make clean && make test && cd ../ && ./benchmarks/test