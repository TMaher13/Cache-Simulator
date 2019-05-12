

#include "CacheSimulator.cpp"


int main() {
  CacheSimulator cache;

  int cacheSizes[4] = {1024, 4096, 65536, 131072};
  int blockSizes[4] = {8, 16, 32, 128};
  char placeType[4] = {'D', '2', '4', 'F'};

  // Clear output before starting
  std::ofstream ofs;
  ofs.open("test.result", std::ofstream::out | std::ofstream::trunc);
  ofs.close();

  // Main testing sequence for all 128 configurations
  for(int cSize : cacheSizes) {
    for(int bSize : blockSizes) {
      for(char pType : placeType) {
        cache = CacheSimulator(cSize, bSize, pType, 'B');
        cache.runSim();
        cache.printCacheInfo();

        cache = CacheSimulator(cSize, bSize, pType, 'T');
        cache.runSim();
        cache.printCacheInfo();
      }
    }
  }


  /* Run this commented block in order to test the association benefits
  cache = CacheSimulator(4096, 16, 'D', 'B');
  cache.runSim();
  cache.printCacheInfo();

  cache = CacheSimulator(4096, 16, '2', 'B');
  cache.runSim();
  cache.printCacheInfo();

  cache = CacheSimulator(4096, 16, '4', 'B');
  cache.runSim();
  cache.printCacheInfo();

  cache = CacheSimulator(4096, 16, 'F', 'B');
  cache.runSim();
  cache.printCacheInfo();
  */


  /* Run this block in order to test the block benefits
  cache = CacheSimulator(1024, 8, 'D', 'T');
  cache.runSim();
  cache.printCacheInfo();

  cache = CacheSimulator(1024, 16, 'D', 'T');
  cache.runSim();
  cache.printCacheInfo();

  cache = CacheSimulator(1024, 32, 'D', 'T');
  cache.runSim();
  cache.printCacheInfo();

  cache = CacheSimulator(1024, 128, 'D', 'T');
  cache.runSim();
  cache.printCacheInfo();
  */

}
