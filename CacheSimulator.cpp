// Class declaratio of Cache Simulator
#include "CacheSimulator.hpp"
#include <cmath>
#include <iterator>
#include <iomanip>
#include <algorithm>

CacheSimulator::CacheSimulator() {
  cacheSize = NULL;
  blockSize = NULL;
  placeType = NULL;
  writePolicy = NULL;
}

// Initialize the constraints of each of the 128 simulations
CacheSimulator::CacheSimulator(int sizeOfCache, int sizeOfBlock, char pType, char wType) {
  cacheSize = sizeOfCache;
  blockSize = sizeOfBlock;
  placeType = pType;
  writePolicy = wType;

  numHits = 0.0;
  numAttempts = 0.0;
  hitRate = 0.0;
  cacheToMem = 0;
  memToCache = 0;

  numBlocks = cacheSize / blockSize;

  cacheMemory = std::vector<Memory_Object>();
  LRU = std::list<int>();

  for(int i = 0; i < numBlocks; i++) {
    cacheMemory.push_back(Memory_Object());
    LRU.push_back(i);
  }

  switch(placeType) {
    case 'D':
      blocksInSet = 1;
      break;
    case '2':
      blocksInSet = 2;
      break;
    case '4':
      blocksInSet = 4;
      break;
    case 'F':
    blocksInSet = numBlocks;
      break;
  }
}

/*
  Script that controls the running of the simulation
  Gets input and either reads or writes
*/
void CacheSimulator::runSim() {
  std::string nextInstr;

  std::string RorW;
  std::string address;

  std::vector<string> lineParts;
  std::ifstream inFile("test.trace");

  while(std::getline(inFile, nextInstr)) {

    std::istringstream iss(nextInstr);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(tokens));

    RorW = tokens.front(); // Whether to read or write
    address = tokens.back(); // Address to read or write to

    int hexAddr;
    std::stringstream ss;
    ss << std::hex << address;
    ss >> hexAddr;

    if(RorW == "read") {
      numHits += readAddr(hexAddr);
    }
    else if(RorW == "write") {
      numHits += writeAddr(hexAddr);
    }
    numAttempts++;
  }

  for(int i = 0; i < numBlocks; i++) {
    if(cacheMemory[i].isDirty()) {
      cacheToMem += blockSize;
    }
  }

  hitRate = numHits / numAttempts;
}


int CacheSimulator::readAddr(int address) {
  switch(placeType) {
    case 'D':
      return readDM(address);
      break;
    case '2':
      return read2W(address);
      break;
    case '4':
      return read4W(address);
      break;
    case 'F':
      return readFA(address);
      break;

  }

  return 1; // returns if a hit or miss
}

int CacheSimulator::readDM(int address) {
  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks) / std::log(2);
  numTag = 32 - numOff - numInd;

   unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
      r |= 1 << i;
  index = address & r;

  r = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
      r |= 1 << i;
  tag = address & r;

  if(cacheMemory.at(index).isValid() == 1) {
    if(cacheMemory.at(index).getTag() == tag) {
      return 1;
    }
    else {
      if(writePolicy == 'B' && cacheMemory.at(index).isDirty())
        cacheToMem += blockSize;

      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(index);
      cacheMemory[index] = addNew;

      memToCache += blockSize;

      return 0;
    }
  }
  else {
    Memory_Object addNew = Memory_Object();
    addNew.setTag(tag);
    addNew.setValid();
    addNew.setIndex(index);
    cacheMemory[index] = addNew;

    memToCache += blockSize;

    return 0;
  }
}

int CacheSimulator::read2W(int address) {

  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks / blocksInSet) / std::log(2);
  numTag = 32 - numOff - numInd;

  unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
    r |= 1 << i;
  index = address & r;
  index = index * blocksInSet;

  unsigned r2 = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
    r2 |= 1 << i;
  tag = address & r2;

  for(int i = index; i < index + blocksInSet; i++) {
    if(cacheMemory[i].isValid() && cacheMemory[i].getTag() == tag) {
      LRU.remove(i);
      LRU.push_back(i);
      return 1;
    }
  }

  for(int i = index; i < index + blocksInSet; i++) {
    if(cacheMemory[i].isValid() == false) {
      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(i);
      cacheMemory[i] = addNew;

      LRU.remove(i);
      LRU.push_back(i);

      memToCache += blockSize;

      return 0;
    }
  }

  int firstRecent = -1;
  int secondRecent = -1;
  std::list<int>::iterator it; // = LRU.begin();

  for(int i = 0; i < numBlocks; i++) {
    it = LRU.begin();
    std::advance(it, i);
    if(*it == index)
      firstRecent = i;
    else if(*it == index+1)
      secondRecent = i;

    if(firstRecent >= 0 && secondRecent >= 0) {
      break;
    }
  }

  if(secondRecent < firstRecent)
    index++;

  if(cacheMemory[index].isDirty()) {
    cacheToMem += blockSize;
  }

  Memory_Object addNew = Memory_Object();
  addNew.setTag(tag);
  addNew.setValid();
  addNew.setIndex(index);
  cacheMemory[index] = addNew;

  LRU.remove(index);
  LRU.push_back(index);

  memToCache += blockSize;

  return 0;
}

int CacheSimulator::read4W(int address) {
  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks / 4) / std::log(2);
  numTag = 32 - numOff - numInd;

  unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
    r |= 1 << i;
  index = address & r;
  index = index * 4;

  unsigned r2 = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
    r2 |= 1 << i;
  tag = address & r2;

  for(int i = index; i < index + 4; i++) {
    if(cacheMemory[i].isValid() && cacheMemory[i].getTag() == tag) {
      LRU.remove(i);
      LRU.push_back(i);
      return 1;
    }
  }

  for(int i = index; i < index + 4; i++) {
    if(cacheMemory[i].isValid() == false) {
      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(i);
      cacheMemory[i] = addNew;

      LRU.remove(i);
      LRU.push_back(i);

      memToCache += blockSize;

      return 0;
    }
  }

  int firstRecent = -1;
  int secondRecent = -1;
  int thirdRecent = -1;
  int fourthRecent = -1;
  std::list<int>::iterator it; // = LRU.begin();

  for(int i = 0; i < numBlocks; i++) {
    it = LRU.begin();
    std::advance(it, i);
    if(*it == index)
      firstRecent = i;
    else if(*it == index+1)
      secondRecent = i;
    else if(*it == index+2)
      thirdRecent = i;
    else if(*it == index+3)
      fourthRecent = i;

    if(firstRecent >= 0 && secondRecent >= 0 && thirdRecent >= 0 && fourthRecent >= 0) {
      break;
    }
  }

  if(std::min(firstRecent, std::min(secondRecent, std::min(thirdRecent, fourthRecent))) == secondRecent)
    index++;
  else if(std::min(firstRecent, std::min(secondRecent, std::min(thirdRecent, fourthRecent))) == thirdRecent)
    index += 2;
  else if(std::min(firstRecent, std::min(secondRecent, std::min(thirdRecent, fourthRecent))) == fourthRecent)
    index += 3;

  if(cacheMemory[index].isDirty()) {
    cacheToMem += blockSize;
  }

  Memory_Object addNew = Memory_Object();
  addNew.setTag(tag);
  addNew.setValid();
  addNew.setIndex(index);
  cacheMemory[index] = addNew;

  LRU.remove(index);
  LRU.push_back(index);

  memToCache += blockSize;

  return 0;
}

int CacheSimulator::readFA(int address) {
  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks) / std::log(2);
  numTag = 32 - numOff - numInd;

   unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
      r |= 1 << i;
  index = address & r;

  r = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
      r |= 1 << i;
  tag = address & r;

  for(int i = 0; i < numBlocks; i++) {
    if(cacheMemory[i].isValid() && cacheMemory[i].getTag() == tag) {
      LRU.remove(i);
      LRU.push_back(i);
      return 1;
    }
  }

  std::list<int>::iterator iter;
  int leastIndex = -1;
  for(iter = LRU.begin(); iter != LRU.end(); iter++) {
    if(!cacheMemory[*iter].isValid()) {
      leastIndex = *iter;

      if(writePolicy == 'B' && isDirtyMemWB(cacheMemory.at(leastIndex)))
        cacheToMem += blockSize;

      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(leastIndex);
      cacheMemory[leastIndex] = addNew;

      memToCache += blockSize;
      return 0;
    }
  }

  leastIndex = LRU.front();
  LRU.remove(LRU.front());
  LRU.push_back(leastIndex);

  if(writePolicy == 'B' && isDirtyMemWB(cacheMemory.at(leastIndex)))
    cacheToMem += blockSize;

  Memory_Object addNew = Memory_Object();
  addNew.setTag(tag);
  addNew.setValid();
  addNew.setIndex(leastIndex);
  cacheMemory[leastIndex] = addNew;
  memToCache += blockSize;

  return 0;
}


int CacheSimulator::writeAddr(int address) {
  switch(placeType) {
    case 'D':
      return writeDM(address);
      break;
    case '2':
      return write2W(address);
      break;
    case '4':
      return write4W(address);
      break;
    case 'F':
      return writeFA(address);
      break;
  }

  return 0; // Should never get here
}

int CacheSimulator::writeDM(int address) {
  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks) / std::log(2);
  numTag = 32 - numOff - numInd;

   unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
      r |= 1 << i;
  index = address & r;

  r = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
      r |= 1 << i;
  tag = address & r;

  if(cacheMemory.at(index).isValid() == 1) {
    if(cacheMemory.at(index).getTag() == tag) {
      if(writePolicy == 'B') {
        cacheMemory[index].setDirty(); // set block as dirty
      }
      else {
        cacheToMem += 4; // immediately write this word to memory
      }
      return 1;
    }
    else {
      if(writePolicy == 'B' && isDirtyMemWB(cacheMemory.at(index)))
        cacheToMem += blockSize;

      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(index);
      cacheMemory[index] = addNew;

      memToCache += blockSize;
      if(writePolicy == 'T') {
        cacheToMem += 4; // immediately write this word to memory
      }
      else { // if policy is WB
        cacheMemory[index].setDirty();
      }

      return 0;
    }
  }
  else {
    Memory_Object addNew = Memory_Object();
    addNew.setTag(tag);
    addNew.setValid();
    addNew.setIndex(index);
    cacheMemory[index] = addNew;

    memToCache += blockSize;
    if(writePolicy == 'T') {
      cacheToMem += 4; // immediately write this word to memory
    }
    else { // if policy is WB
      cacheMemory[index].setDirty();
    }
    return 0;
  }
}

int CacheSimulator::write2W(int address) {

  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks / blocksInSet) / std::log(2);
  numTag = 32 - numOff - numInd;

  unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
      r |= 1 << i;
  index = address & r;
  index = index * blocksInSet;

  unsigned r2 = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
      r2 |= 1 << i;
  tag = address & r2;

  for(int i = index; i < index + blocksInSet; i++) {
    if(cacheMemory[i].isValid() && cacheMemory[i].getTag() == tag) {
      if(writePolicy == 'B') {
        cacheMemory[i].setDirty(); // set block as dirty
      }
      else
        cacheToMem += 4; // immediately write this word to memory

      LRU.remove(i);
      LRU.push_back(i);
      return 1;
    }
  }

  for(int i = index; i < index + blocksInSet; i++) {
    if(cacheMemory[i].isValid() == false) {
      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(i);
      cacheMemory[i] = addNew;

      LRU.remove(i);
      LRU.push_back(i);
      if(writePolicy == 'B') {
        cacheMemory[i].setDirty(); // set block as dirty
      }
      else
        cacheToMem += 4; // immediately write this word to memory

      memToCache += blockSize;

      return 0;
    }
  }

  int firstRecent = -1;
  int secondRecent = -1;
  std::list<int>::iterator it; // = LRU.begin();

  for(int i = 0; i < numBlocks; i++) {
    it = LRU.begin();
    std::advance(it, i);
    if(*it == index)
      firstRecent = i;
    else if(*it == index+1)
      secondRecent = i;

    if(firstRecent >= 0 && secondRecent >= 0) {
      break;
    }
  }

  if(secondRecent < firstRecent)
    index++;

  if(cacheMemory[index].isDirty()) {
    cacheToMem += blockSize;
  }

  Memory_Object addNew = Memory_Object();
  addNew.setTag(tag);
  addNew.setValid();
  addNew.setIndex(index);
  cacheMemory[index] = addNew;

  LRU.remove(index);
  LRU.push_back(index);

  memToCache += blockSize;
  if(writePolicy == 'B') {
    cacheMemory[index].setDirty(); // set block as dirty
  }
  else
    cacheToMem += 4; // immediately write this word to memory

  return 0;
}

int CacheSimulator::write4W(int address) {
  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks / 4) / std::log(2);
  numTag = 32 - numOff - numInd;

  unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
      r |= 1 << i;
  index = address & r;
  index = index * 4;

  unsigned r2 = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
      r2 |= 1 << i;
  tag = address & r2;

  for(int i = index; i < index + 4; i++) {
    if(cacheMemory[i].isValid() && cacheMemory[i].getTag() == tag) {
      if(writePolicy == 'B') {
        cacheMemory[i].setDirty(); // set block as dirty
      }
      else
        cacheToMem += 4; // immediately write this word to memory

      LRU.remove(i);
      LRU.push_back(i);
      return 1;
    }
  }

  for(int i = index; i < index + 4; i++) {
    if(cacheMemory[i].isValid() == false) {
      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(i);
      cacheMemory[i] = addNew;

      LRU.remove(i);
      LRU.push_back(i);
      if(writePolicy == 'B') {
        cacheMemory[i].setDirty(); // set block as dirty
      }
      else
        cacheToMem += 4; // immediately write this word to memory

      memToCache += blockSize;

      return 0;
    }
  }

  int firstRecent = -1;
  int secondRecent = -1;
  int thirdRecent = -1;
  int fourthRecent = -1;
  std::list<int>::iterator it; // = LRU.begin();

  for(int i = 0; i < numBlocks; i++) {
    it = LRU.begin();
    std::advance(it, i);
    if(*it == index)
      firstRecent = i;
    else if(*it == index+1)
      secondRecent = i;
    else if(*it == index+2)
      thirdRecent = i;
    else if(*it == index+3)
      fourthRecent = i;

    if(firstRecent >= 0 && secondRecent >= 0 && thirdRecent >= 0 && fourthRecent >= 0) {
      break;
    }
  }

  if(std::min(firstRecent, std::min(secondRecent, std::min(thirdRecent, fourthRecent))) == secondRecent) {
    index++;
  }
  else if(std::min(firstRecent, std::min(secondRecent, std::min(thirdRecent, fourthRecent))) == thirdRecent) {
    index += 2;
  }
  else if(std::min(firstRecent, std::min(secondRecent, std::min(thirdRecent, fourthRecent))) == fourthRecent) {
    index += 3;
  }

  if(cacheMemory[index].isDirty()) {
    cacheToMem += blockSize;
  }

  Memory_Object addNew = Memory_Object();
  addNew.setTag(tag);
  addNew.setValid();
  addNew.setIndex(index);
  cacheMemory[index] = addNew;

  LRU.remove(index);
  LRU.push_back(index);

  memToCache += blockSize;
  if(writePolicy == 'B') {
    cacheMemory[index].setDirty(); // set block as dirty
  }
  else
    cacheToMem += 4; // immediately write this word to memory

  return 0;
}

int CacheSimulator::writeFA(int address) {
  int index, tag;
  int numOff, numInd, numTag;

  numOff = std::log(blockSize) / std::log(2);
  numInd = std::log(numBlocks) / std::log(2);
  numTag = 32 - numOff - numInd;

   unsigned r = 0;
  for (unsigned i = numOff; i < numOff + numInd; i++)
      r |= 1 << i;
  index = address & r;

  r = 0;
  for (unsigned i=numOff + numInd; i < 32; i++)
      r |= 1 << i;
  tag = address & r;

  for(int i = 0; i < numBlocks; i++) {
    if(cacheMemory[i].isValid() && cacheMemory[i].getTag() == tag) {
      if(writePolicy == 'T') {
        cacheToMem += 4; // immediately write this word to memory
      }
      else { // if policy is WB
        cacheMemory[i].setDirty();
      }

      LRU.remove(i);
      LRU.push_back(i);
      return 1;
    }
  }

  std::list<int>::iterator iter;
  int leastIndex = -1;
  for(iter = LRU.begin(); iter != LRU.end(); iter++) {
    if(!cacheMemory[*iter].isValid()) {
      leastIndex = *iter;

      if(writePolicy == 'B' && isDirtyMemWB(cacheMemory.at(leastIndex)))
        cacheToMem += blockSize;

      Memory_Object addNew = Memory_Object();
      addNew.setTag(tag);
      addNew.setValid();
      addNew.setIndex(leastIndex);
      cacheMemory[leastIndex] = addNew;

      memToCache += blockSize;
      if(writePolicy == 'T') {
        cacheToMem += 4; // immediately write this word to memory
      }
      else { // if policy is WB
        cacheMemory[leastIndex].setDirty();
      }

      return 0;
    }
  }

  leastIndex = LRU.front();
  LRU.remove(LRU.front());
  LRU.push_back(leastIndex);

  if(writePolicy == 'B' && isDirtyMemWB(cacheMemory.at(leastIndex)))
    cacheToMem += blockSize;

  Memory_Object addNew = Memory_Object();
  addNew.setTag(tag);
  addNew.setValid();
  addNew.setIndex(leastIndex);
  cacheMemory[leastIndex] = addNew;
  memToCache += blockSize;
  if(writePolicy == 'T') {
    cacheToMem += 4; // immediately write this word to memory
  }
  else { // if policy is WB
    cacheMemory[leastIndex].setDirty();
  }

  return 0;
}


bool CacheSimulator::isDirtyMemWB(Memory_Object memObj) {
  return memObj.isDirty();
}

void CacheSimulator::printCacheInfo() {
  std::ofstream outFile; //("test.result");

  outFile.open("test.result", std::ios_base::app);

  outFile << cacheSize << "\t" << blockSize;

  switch(placeType) {
    case 'D':
      if(writePolicy == 'B')
        outFile << "\tDM\tWB\t";
      else
        outFile << "\tDM\tWT\t";
      break;
    case '2':
      if(writePolicy == 'B')
        outFile << "\t2W\tWB\t";
      else
        outFile << "\t2W\tWT\t";
      break;
    case '4':
      if(writePolicy == 'B')
        outFile << "\t4W\tWB\t";
      else
        outFile << "\t4W\tWT\t";
      break;
    case 'F':
      if(writePolicy == 'B')
        outFile << "\tFA\tWB\t";
      else
        outFile << "\tFA\tWT\t";
      break;
  }

  outFile << std::fixed << std::setprecision(2) << hitRate << "\t" << memToCache << "\t" << cacheToMem << "\t" << blocksInSet << '\n';

  outFile.close();
  return;
}
