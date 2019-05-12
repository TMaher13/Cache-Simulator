# Cache-Simulator
A simulator for testing direct mapped, 2 and 4 way set associative, and fully associative cache systems

## Files required for project

* simRunner.cpp
* CacheSimulator.hpp
* CacheSimulator.cpp

## Compiler and OS environment

My project was build on a Windows machine with the Microsoft C++ Compiler (MSVC).

## Compile and Run Instructions

To compile and run simulations, create a trace file named "test.trace" and place it in the directory with the above files.

When running from the command line in the correct directory, enter the following to start the simulations:

```
cl /EHsc simRunner.cpp
```

And then

```
.\simRunner.exe
```

## Reading Results

1024 8 FA WB 0.91 1024 512 128

The above numbers represent the following:
	1. Cache size (in bytes)
	2. Block size (in bytes)
	3. Mapping type (DM, 2W, 4W, and FA for direct-mapped, 2-way and 4-way set associative, and fully-associative caches respectively)
	4. Write policy (WB, WT)
	5. Hit rate (two decimal points)
	6. Total bytes transferred from memory to the cache
	7. Total bytes transferred from cache to memory
	8. Number of blocks within each block set.


	
