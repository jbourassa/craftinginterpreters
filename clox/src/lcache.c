#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "lcache.h"

#define LCACHE_MAX_LOAD 0.75

void initLcache(Lcache* lcache) {
  lcache->count = 0;
  lcache->capacity = 0;
  lcache->entries = NULL;
}

void freeLcache(Lcache* lcache) {
  for (int i = 0; i < lcache->capacity; i++) {
    LcacheEntry* entry = &lcache->entries[i];
    if (entry->locals.values) {
      FREE_ARRAY(LcacheValue, entry->locals.values, entry->locals.capacity);
    }
  }

  FREE_ARRAY(LcacheEntry, lcache->entries, lcache->capacity);
  initLcache(lcache);
}

static LcacheEntry* findEntry(LcacheEntry* entries, int capacity, ObjString* key) {
  uint32_t index = key->hash % capacity;
  for (;;) {
    LcacheEntry* entry = &entries[index];
    if (entry->key == key || entry->key == NULL) {
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

LcacheValues* lcacheGet(Lcache* lcache, ObjString* key) {
  if (lcache->count == 0) return NULL;

  LcacheEntry* entry = findEntry(lcache->entries, lcache->capacity, key);
  if (!entry->key) return NULL;
  return &entry->locals;
}

static void adjustCapacity(Lcache* lcache, int capacity) {
  LcacheEntry* entries = ALLOCATE(LcacheEntry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].locals.values = NULL;
    entries[i].locals.capacity = 0;
    entries[i].locals.count = 0;
  }

  lcache->count = 0;
  for (int i = 0; i < lcache->capacity; i++) {
    LcacheEntry* entry = &lcache->entries[i];
    if (entry->key == NULL) continue;

    LcacheEntry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->locals = entry->locals;
    lcache->count++;
  }

  FREE_ARRAY(LcacheEntry, lcache->entries, lcache->capacity);
  lcache->entries = entries;
  lcache->capacity = capacity;
}

void lcacheAdd(Lcache* lcache, ObjString* key, int depth, int index) {
  if (lcache->count + 1 > lcache->capacity * LCACHE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(lcache->capacity);
    adjustCapacity(lcache, capacity);
  }

  LcacheEntry* entry = findEntry(lcache->entries, lcache->capacity, key);
  if (entry->locals.values == NULL) lcache->count++;
  LcacheValues* locals = &entry->locals;

  if (locals->capacity < locals->count + 1) {
    int oldCapacity = locals->capacity;
    locals->capacity = GROW_CAPACITY(oldCapacity);
    locals->values = GROW_ARRAY(LcacheValue, locals->values, oldCapacity, locals->capacity);
  }

  locals->values[locals->count].depth = depth;
  locals->values[locals->count].index = index;
  locals->count = locals->count + 1;

  entry->key = key;
}
