#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"

#define HEAP_CELLS 140                            // Total number of cells in the heap map
#define CELL_SIZE 8                               // Size of each cell in bytes (8 bytes)
#define TOTAL_HEAP_SIZE (HEAP_CELLS * CELL_SIZE)  // Total heap size in bytes (1120 bytes)

#define CELL_FREE            0b00000000           // Cell is free and available for allocation
#define CELL_INUSE           0b00000001           // Cell is currently in use
#define CELL_INUSE_CONTUNIE  0b00000011           // Cell is in use and part of a continued block
#define CELL_INUSE_LAST      0b00000111           // Last cell in a block that is in use

// API declarations
extern void heap_init();                          // Initialize the heap
extern void* malloc(uint16_t size);               // Allocate memory of given size
extern char free(uint8_t* base_addr);             // Free the allocated memory at the given address

#endif  // MALLOC_H
