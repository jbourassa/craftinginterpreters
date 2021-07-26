#include "common.h"
#include "chunk.h"
#include "debug.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 1);
  writeChunk(&chunk, constant, 1);
  writeChunk(&chunk, OP_RETURN, 1);

  int constant2 = addConstant(&chunk, 2);
  writeChunk(&chunk, OP_CONSTANT, 2);
  writeChunk(&chunk, constant2, 2);
  writeChunk(&chunk, OP_RETURN, 2);

  writeChunk(&chunk, OP_RETURN, 3);

  printf("Chunk: %d, Line: %d - exp: 1 \n", 0, getLine(&chunk, 0));
  printf("Chunk: %d, Line: %d - exp: 1 \n", 1, getLine(&chunk, 1));
  printf("Chunk: %d, Line: %d - exp: 1 \n", 2, getLine(&chunk, 2));


  printf("\n\n");

  printf("Chunk: %d, Line: %d - exp: 2 \n", 3, getLine(&chunk, 3));
  printf("Chunk: %d, Line: %d - exp: 2 \n", 4, getLine(&chunk, 4));
  printf("Chunk: %d, Line: %d\n", 5, getLine(&chunk, 5));

  printf("Chunk: %d, Line: %d - exp : 3\n", 6, getLine(&chunk, 6));

  disassembleChunk(&chunk, "test chunk");

  freeChunk(&chunk);

  return 0;
}

