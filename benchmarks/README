How to run the benchmarking code:
-------------------------
0. Move this code to an iLab machine and decompress it

1. First, do a make clean and make
	$ make clean
	$ make

2. Before running the benchmark, run 
	$ ./genRecord.sh

	... genRecord generates data for external_cal.

3. Running benchmarks
	invoke with one numeric paramer, the number of threads to run:
	$   ./external_cal 6
	$ ./parallelCal 6
	$ ./vector_multiply 6

     ... in order to start up the benchmark with 6 threads

Make sure to test your code with different user-level thread counts and measure performance. 
We will test your code for a large number (50-100) of user-level threads.


Checking correctness
-----------------------

You can check the correctness of code in the following ways.

1.) For each benchmark, you would see the calculated sum and the verified sum to be the same.

Sample output:

        running time: 1373 micro-seconds
        sum is: 83842816
        verified sum is: 83842816


2.) Disable "USE_MYTHREAD" in mypthread.h and compile the library again before compiling the benchmark. 
The code will use "pthread" library by default.

//#define USE_MYTHREAD 1
