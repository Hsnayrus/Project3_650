#include <cstdlib>
#include <vector>

typedef struct Potato {
  size_t hops;
  int traceVector[512];
  size_t vecSize;
} potato_t;