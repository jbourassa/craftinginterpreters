#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_RETURN,
} OpCode;

typedef struct {
  int capacity;
  int count;
  int* line_no;
  int* occurrences;
} LinesMapping;

typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  ValueArray constants;
  LinesMapping lines;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int index);

#endif
