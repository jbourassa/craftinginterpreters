#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

static void initLineMapping(LinesMapping* lines) {
  lines->line_no = NULL;
  lines->occurrences = NULL;
  lines->capacity = 0;
  lines->count = 0;
}

static void freeLineMapping(LinesMapping* lines) {
  FREE_ARRAY(int, lines->line_no, lines->capacity);
  FREE_ARRAY(int, lines->occurrences, lines->capacity);
  initLineMapping(lines);
}

static void lineSeen(LinesMapping* lines, int line_no) {
  bool resize = lines->capacity == 0 ||
    (lines->line_no[lines->count - 1] != line_no && lines->capacity < lines->count + 1);

  if (resize) {
    int oldCapacity = lines->capacity;
    lines->capacity = GROW_CAPACITY(oldCapacity);
    lines->line_no = GROW_ARRAY(int, lines->line_no, oldCapacity, lines->capacity);
    lines->occurrences = GROW_ARRAY(int, lines->occurrences, oldCapacity, lines->capacity);
  }

  if (lines->line_no[lines->count - 1] == line_no) {
    lines->occurrences[lines->count - 1]++;
  } else {
    lines->line_no[lines->count] = line_no;
    lines->occurrences[lines->count] = 1;
    lines->count++;
  }
}

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initLineMapping(&chunk->lines);
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeValueArray(&chunk->constants);
  freeLineMapping(&chunk->lines);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  lineSeen(&chunk->lines, line);
  chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int chunkIndex) {
  LinesMapping* lines = &chunk->lines;

  int chunksSeen = 0;
  for (int i = 0; i < lines->count; i++) {
    chunksSeen += lines->occurrences[i];
    if (chunksSeen > chunkIndex) {
      return lines->line_no[i];
    }
  }

  return 0;
}
