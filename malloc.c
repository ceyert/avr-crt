#include "gcc_builtins.h"
#include "malloc.h"

extern uint8_t __data_end;
static uint8_t* bitmap_base_addr = &__data_end;

static uint8_t* heap_free_base_addr;

static void apply_alignemt(uint16_t* size_in_bytes)
{
    if (*size_in_bytes < PER_CELL_SIZE)
    {
        *size_in_bytes = PER_CELL_SIZE;
        return;
    }

    // apply alignment to PER_CELL_SIZE
    {
        uint16_t remainder = *size_in_bytes % PER_CELL_SIZE;
        uint16_t adder = PER_CELL_SIZE - remainder;
        *size_in_bytes += adder;
    }
}

static void* apply_occupy(uint8_t base_idx, uint8_t total_req_cells)
{
    void* allocated_ptr = heap_free_base_addr;
    
    if (total_req_cells == 1)
    {
        bitmap_base_addr[base_idx] = BITMAP_CELL_INUSE;

        heap_free_base_addr += PER_CELL_SIZE;
        return allocated_ptr;
    }

    bitmap_base_addr[base_idx++] = BITMAP_CELL_INUSE;

    while (base_idx < total_req_cells - 1) // last cell will be used for inuse_last
    {
        bitmap_base_addr[base_idx++] = BITMAP_CELL_INUSE_CONTUNIE;
    }

    bitmap_base_addr[base_idx] = BITMAP_CELL_INUSE_LAST;

    heap_free_base_addr += (total_req_cells * PER_CELL_SIZE);
    return allocated_ptr;
}

void malloc_init()
{
    memset(bitmap_base_addr, BITMAP_CELL_FREE, BITMAP_SIZE);

    heap_free_base_addr = bitmap_base_addr + (PER_CELL_SIZE * BITMAP_SIZE);
}

void* malloc(uint16_t size_in_bytes)
{
    apply_alignemt(&size_in_bytes);

    uint8_t total_req_cells = size_in_bytes / PER_CELL_SIZE;

    uint8_t encounted_cells = 0;
    uint8_t found = 0;
    uint8_t base_idx = 0;

    uint8_t idx = 0;

    while (idx < BITMAP_SIZE)
    {
        encounted_cells++;

        if (bitmap_base_addr[idx] != BITMAP_CELL_FREE)
        {
            encounted_cells = 0;
            base_idx = idx + 1;
        }

        if (encounted_cells == total_req_cells)
        {
            found = 1;
            break;
        }

        idx++;
    }

    if (!found)
    {
        return (void*)-1;
    }

    return apply_occupy(base_idx, total_req_cells);
}

void free(uint8_t* base_addr)
{
    // TODO
}
