

In q2 there were alot of confusions regarding how many number of processes, as the segment size was fixed. I assumed that the number of processes would be decided by us using the formula given below.

nporcesses = arraySize/segmentSize

But the problem was how to tell MPI that I need this many processes. As it gets the number of processes in the command line.
One solution is to write bash code which I did. 

This bash command takes 2 integer arguments 
1. sqrt(ArraySize)
2. sqrt(SegmentSize)

it can be run as :
./script.sh 11 8

This will compile and run q2. The array size will be 2^11 and segment size will be 2^5.



--> Another problem is the limit on leaf nodes. MPI Library puts a limit on the number of leaf nodes. So The code works fine but this restriction prevents the code from running in some situation. Due to this I could not caluclate GFLOPS in some cases and hence could not analyze the performance.


While merging I have tried to implement the following logic which increases the performance from traditional merging.

suppose we have 64 threads and the array is divided in those threads. All 64 will sort their part of the array. Now 1 thread will merge 2 sorted arrays which means that now 32 threads will be used, 16 in the next step and so on. Finally 1 thread will merge 2 large arrays. This reduction in the number of active threads is due to the fact that each merge operation requires reading and writing to the same array, and having multiple threads accessing the same array can result in conflicts and race conditions The problem is that at each step the number of threads involved in the computation are divided in half.

But the advantage is that in a setup like cluster the communication between the nodes is minimum and they work on their own part of the array.