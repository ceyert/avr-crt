#include "gcc_builtins.h"
#include "malloc.h"


// External memory location symbol for the end of data section from CRT
extern uint8_t __data_end;
static uint8_t* heapmap_base_addr = &__data_end;  // Base address of the heap map, initialized to end of data

// Pointers to manage heap and free memory space
static uint8_t* heap_base_addr = 0;               // Base address of the heap
static uint8_t* heap_free_base_addr = 0;          // Pointer to the next free space in the heap

// Function to align the requested size to the nearest multiple of CELL_SIZE
static void apply_alignment(uint16_t* size_in_bytes)
{
    if (*size_in_bytes < CELL_SIZE) // If size is less than one cell, round up to one cell
    {
        *size_in_bytes = CELL_SIZE;
        return;
    }

    // Align the size to the nearest multiple of CELL_SIZE
    {
        uint16_t remainder = *size_in_bytes % CELL_SIZE; // Calculate the remainder
        uint16_t adder = CELL_SIZE - remainder;          // Calculate how much to add to align
        *size_in_bytes += adder;                         // Adjust the size
    }
}

// Function to mark cells as occupied in the heap map and return the allocated memory address
static void* apply_occupy(uint8_t base_idx, uint8_t total_req_cells)
{
    void* allocated_ptr = heap_free_base_addr; // Starting address of the allocated memory

    if (total_req_cells == 1) // If only one cell is required
    {
        heapmap_base_addr[base_idx] = CELL_INUSE | CELL_INUSE_LAST; // Mark cell as in use and last in block

        // Move the free base address to the next cell
        heap_free_base_addr += CELL_SIZE;

        return allocated_ptr;
    }

    heapmap_base_addr[base_idx++] = CELL_INUSE; // Mark the first cell as in use

    // Mark all intermediate cells as continued in use, except the last one
    while (base_idx < total_req_cells - 1)
    {
        heapmap_base_addr[base_idx++] = CELL_INUSE_CONTUNIE;
    }

    heapmap_base_addr[base_idx] = CELL_INUSE_LAST; // Mark the last cell as in use and last in block

    // Move the free base address to after the allocated block
    heap_free_base_addr += (total_req_cells * CELL_SIZE);

    return allocated_ptr; // Return the starting address of the allocated memory
}

// Function to setting all cells as free & initialize heap_base_addr and heap_free_base_addr addresses
void heap_init()
{
    memset(heapmap_base_addr, CELL_FREE, HEAP_CELLS); // Initialize all cells as free

    heap_base_addr = heapmap_base_addr + HEAP_CELLS + sizeof(uint8_t); // Set heap base address just after heap map
    heap_free_base_addr = heap_base_addr; // Initialize the free base address to the start of the heap
}

// Function to allocate memory of the requested size
void* malloc(uint16_t size_in_bytes)
{
    apply_alignment(&size_in_bytes); // Align the requested size

    uint8_t total_req_cells = size_in_bytes / CELL_SIZE; // Calculate the number of cells required

    uint8_t encounted_cells = 0; // Counter for contiguous free cells
    uint8_t found = 0;           // Flag to indicate if a suitable block is found
    uint8_t base_idx = 0;        // Base index for allocation

    uint8_t idx = 0; // Index to traverse the heap map

    // Search for a contiguous block of free cells
    while (idx < HEAP_CELLS)
    {
        encounted_cells++; // Increment free cell count

        if (heapmap_base_addr[idx] != CELL_FREE) // Reset count if cell is not free
        {
            encounted_cells = 0;
            base_idx = idx + 1;
        }

        if (encounted_cells == total_req_cells) // Block found
        {
            found = 1;
            break;
        }

        idx++; // Move to next cell
    }

    if (!found) // If no suitable block is found, return NULL
    {
        return 0x0;
    }

    return apply_occupy(base_idx, total_req_cells); // Mark the block as occupied and return its address
}

// Function to check if the allocated pointer is valid
static uint8_t check_allocated_ptr_valid(uint8_t allocated_ptr_idx)
{
    if (heapmap_base_addr[allocated_ptr_idx] == CELL_INUSE || heapmap_base_addr[allocated_ptr_idx] == (CELL_INUSE | CELL_INUSE_LAST))
    {
        return 1; // Valid if the cell is in use or in use as the last cell
    }

    return 0; // Otherwise, it is invalid
}

// Function to free the memory block starting from the given index
static char apply_free(uint8_t allocated_ptr_idx)
{
    uint8_t total_allocated_cells = 0; // Counter for allocated cells to be freed

    // Traverse and free each cell in the block
    while (heapmap_base_addr[allocated_ptr_idx] != CELL_FREE)
    {
        // Free only one cell used
        if (heapmap_base_addr[allocated_ptr_idx] == (CELL_INUSE | CELL_INUSE_LAST))
        {
            heapmap_base_addr[allocated_ptr_idx] = CELL_FREE; 
            break;
        }

        // If the current cell is the last cell of the block
        if (heapmap_base_addr[allocated_ptr_idx] == CELL_INUSE_LAST)
        {
            heapmap_base_addr[allocated_ptr_idx] = CELL_FREE; 
            break;
        }

        // Safety check to avoid freeing more cells than the heap has
        if (total_allocated_cells > HEAP_CELLS)
        {
            heap_init(); // Reinitialize the heap if something goes wrong
            return 0; 
        }

        heapmap_base_addr[allocated_ptr_idx] = CELL_FREE; // Mark the cell as free

        allocated_ptr_idx--; // Move to the previous cell
        total_allocated_cells++; // Increment the count of freed cells
    }

    // Shrink the free base address back by the number of freed cells
    heap_free_base_addr -= (total_allocated_cells * CELL_SIZE);

    return 1; // Return success
}

// Function to free allocated memory starting from a given base address
char free(uint8_t* allocated_base_addr)
{
    // Check if the provided pointer is within the heap range
    if (allocated_base_addr < heap_base_addr || allocated_base_addr >= (heap_base_addr + HEAP_CELLS * CELL_SIZE))
    {
        return -1; // Invalid pointer
    }

    uint8_t allocated_ptr_idx = (allocated_base_addr - heap_base_addr) / CELL_SIZE; // Calculate the index of the cell

    if (check_allocated_ptr_valid(allocated_ptr_idx) == 0) // Verify if the pointer is valid
    {
        return -1;  // Invalid pointer
    }

    return apply_free(allocated_ptr_idx); // Free the allocated memory block and return the status
}
