# Cache-Simulator
A simulator for testing direct mapped, 2 and 4 way set associative, and fully associative cache systems. Simulation calculates number of hits and misses for a given .trace file input

## Files required for project

* simRunner.cpp
* CacheSimulator.hpp
* CacheSimulator.cpp

## Compiler and OS environment

My project was built on a Windows machine with the Microsoft C++ Compiler (MSVC).

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

Each line has the following syntax (memory sizes, chache to memory, and memory to cache are in bytes):

[cache size] [block size] [cache type] [write policy] [hit rate] [memory to cache] [cache to memory] [blocks per set]


	
