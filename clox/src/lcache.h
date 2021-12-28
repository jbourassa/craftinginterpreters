#ifndef clox_lcache_h
#define clox_lcache_h

#include "common.h"
#include "value.h"

typedef struct {
  int depth;
  int index;
} LcacheValue;

typedef struct {
  int capacity;
  int count;
  LcacheValue* values;
} LcacheValues;

typedef struct {
  ObjString* key;
  LcacheValues locals;
} LcacheEntry;

typedef struct {
  int count;
  int capacity;
  LcacheEntry* entries;
} Lcache;

void initLcache(Lcache* lcache);
void freeLcache(Lcache* lcache);
void lcacheAdd(Lcache* lcache, ObjString* key, int depth, int index);
LcacheValues* lcacheGet(Lcache* lcache, ObjString* key);

#endif
