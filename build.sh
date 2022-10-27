#!/bin/bash

make clean && make all && cd benchmarks && make clean && make test && cd ../ && ./benchmarks/test

