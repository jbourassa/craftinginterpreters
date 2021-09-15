#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
  initVM();

  Chunk chunk;
  initChunk(&chunk);

  Chunk chunk2;
  initChunk(&chunk2);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);
  for(int i = 0; i < 10000000; i++) writeChunk(&chunk, OP_NEGATE, 123);
  writeChunk(&chunk, OP_RETURN, 123);


  int constant2 = addConstant(&chunk2, 1.2);
  writeChunk(&chunk2, OP_CONSTANT, 123);
  writeChunk(&chunk2, constant2, 123);
  for(int i = 0; i < 10000000; i++) writeChunk(&chunk2, OP_NEGATE_IP, 123);
  writeChunk(&chunk2, OP_RETURN, 123);


  clock_t start, end;
  double cpu_time_used;

  start = clock();
  interpret(&chunk);
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("\nOP_NEGATE: %f\n", cpu_time_used);

  start = clock();
  interpret(&chunk2);
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("\nOP_NEGAIP: %f\n", cpu_time_used);

  return 0;
}

