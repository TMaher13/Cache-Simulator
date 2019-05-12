#ifndef CACHE_SIMULATOR_HPP
#define CACHE_SIMULATOR_HPP

// Cache Simulator class used in Project 3 for ECE 2500
// Author: Thomas Maher
// Date: May 11, 2019
// Version: 1.0

#include <vector>
#include <string>
#include <stdio.h>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>

using std::string;

// Each space in the cache is occupied by this memory object type
struct Memory_Object {
  bool valid;
  bool dirty;

  int index;
  int tag;

  Memory_Object() {
    valid = false;
    dirty = false;
    index = 0;
    tag = 0;
  }

  int getIndex() {
    return index;
  }

  int getTag() {
    return tag;
  }

  bool isValid() {
    return valid;
  }
  bool isDirty() {
    return dirty;
  }

  void setIndex(int newIndex) {
    index = newIndex;
  }

  void setTag(int newTag) {
    tag = newTag;
  }

  void setValid() {
    valid = true;
  }

  void setDirty() {
    dirty = true;
  }

};

class CacheSimulator {

  private:

    // Basic configurations
    int cacheSize;
    int blockSize;
    char placeType;
    char writePolicy;
    std::vector<Memory_Object> cacheMemory;
    std::list<int> LRU;

    // Secondary calculations
    int numBlocks;
    int blocksInSet;
    double numHits;
    double numAttempts;
    double hitRate;
    int cacheToMem;
    int memToCache;

  public:

    CacheSimulator();

    // Initialize the constraints of each of the 128 simulations
    CacheSimulator(int sizeOfCache, int sizeOfBlock, char pType, char wType);

    // Top-level function that handles the input file and reads to cache
    void runSim();

    int readAddr(int address);
    int readDM(int address);
    int read2W(int address);
    int read4W(int address);
    int readFA(int address);

    int writeAddr(int address);
    int writeDM(int address);
    int write2W(int address);
    int write4W(int address);
    int writeFA(int address);

    bool isDirtyMemWB(Memory_Object memObj);

    void printCacheInfo();



};

#endif
