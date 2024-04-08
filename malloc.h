#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"


#define BITMAP_SIZE 140                                 // total bitmap size
#define PER_CELL_SIZE 8                                 // 8 bytes each CELL
#define TOTAL_HEAP_SIZE (BITMAP_SIZE * PER_CELL_SIZE)   // 1120 bytes heap in total

#define BITMAP_CELL_FREE            0b00000000
#define BITMAP_CELL_INUSE           0b00000001
#define BITMAP_CELL_INUSE_CONTUNIE  0b00000011
#define BITMAP_CELL_INUSE_LAST      0b00000111

extern void malloc_init();

extern void* malloc(uint16_t size);

extern void free(uint8_t *base_addr);

#endif // MALLOC_H
