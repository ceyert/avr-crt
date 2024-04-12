#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"

#define HEAP_CELLS 140                            // total heapmap size
#define CELL_SIZE 8                               // 8 bytes each CELL
#define TOTAL_HEAP_SIZE (HEAP_CELLS * CELL_SIZE)  // 1120 bytes heap in total

#define CELL_FREE            0b00000000
#define CELL_INUSE           0b00000001
#define CELL_INUSE_CONTUNIE  0b00000011
#define CELL_INUSE_LAST      0b00000111

extern void heap_init();

extern void* malloc(uint16_t size);

extern char free(uint8_t* base_addr);

#endif  // MALLOC_H
