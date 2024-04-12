#include "gcc_builtins.h"
#include "malloc.h"

extern uint8_t __data_end;
static uint8_t* heapmap_base_addr = &__data_end;

static uint8_t* heap_base_addr = 0;
static uint8_t* heap_free_base_addr = 0;

static void apply_alignment(uint16_t* size_in_bytes)
{
    if (*size_in_bytes < CELL_SIZE)
    {
        *size_in_bytes = CELL_SIZE;
        return;
    }

    // apply alignment to CELL_SIZE
    {
        uint16_t remainder = *size_in_bytes % CELL_SIZE;
        uint16_t adder = CELL_SIZE - remainder;
        *size_in_bytes += adder;
    }
}

static void* apply_occupy(uint8_t base_idx, uint8_t total_req_cells)
{
    void* allocated_ptr = heap_free_base_addr;

    if (total_req_cells == 1)
    {
        heapmap_base_addr[base_idx] = CELL_INUSE;

        // expand heap free base address
        heap_free_base_addr += CELL_SIZE;

        return allocated_ptr;
    }

    heapmap_base_addr[base_idx++] = CELL_INUSE;

    while (base_idx < total_req_cells - 1)  // last cell will be used for inuse_last
    {
        heapmap_base_addr[base_idx++] = CELL_INUSE_CONTUNIE;
    }

    heapmap_base_addr[base_idx] = CELL_INUSE_LAST;

    // expand heap free base address
    heap_free_base_addr += (total_req_cells * CELL_SIZE);

    return allocated_ptr;
}

void heap_init()
{
    memset(heapmap_base_addr, CELL_FREE, HEAP_CELLS);

    heap_base_addr = heapmap_base_addr + HEAP_CELLS + sizeof(uint8_t); // point 1 byte below after heap map
    heap_free_base_addr = heap_base_addr;
}

void* malloc(uint16_t size_in_bytes)
{
    apply_alignment(&size_in_bytes);

    uint8_t total_req_cells = size_in_bytes / CELL_SIZE;

    uint8_t encounted_cells = 0;
    uint8_t found = 0;
    uint8_t base_idx = 0;

    uint8_t idx = 0;

    while (idx < HEAP_CELLS)
    {
        encounted_cells++;

        if (heapmap_base_addr[idx] != CELL_FREE)
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
        return 0x0;
    }

    return apply_occupy(base_idx, total_req_cells);
}

static uint8_t check_allocated_ptr_valid(uint8_t allocated_ptr_idx)
{
    return heapmap_base_addr[allocated_ptr_idx] == CELL_INUSE ? 1 : 0;
}

static char apply_free(uint8_t allocated_ptr_idx)
{
    uint8_t total_allocated_cells = 0;

    while (allocated_ptr_idx >= 0)
    {
        if (heapmap_base_addr[allocated_ptr_idx] == CELL_FREE)
        {
            break;
        }

        if (heapmap_base_addr[allocated_ptr_idx] == CELL_INUSE_LAST)
        {
            heapmap_base_addr[allocated_ptr_idx] = CELL_FREE; 
            break;
        }

        if (total_allocated_cells > HEAP_CELLS) // samething goes wrong
        {
            return 0; 
        }
        
        heapmap_base_addr[allocated_ptr_idx] = CELL_FREE;

        allocated_ptr_idx--;
        total_allocated_cells++;
    }
    
    // shink heap free base address
    heap_free_base_addr -= (total_allocated_cells * CELL_SIZE);

    return 1;
}

char free(uint8_t* allocated_base_addr)
{
    if (allocated_base_addr < heap_base_addr || allocated_base_addr >= (heap_base_addr + HEAP_CELLS * CELL_SIZE))
    {
        return -1; // Invalid pointer
    }

    uint8_t allocated_ptr_idx = (allocated_base_addr - heap_base_addr) / CELL_SIZE;

    if (check_allocated_ptr_valid(allocated_ptr_idx) == 0)
    {
        return -1;  // Invalid pointer
    }

    return apply_free(allocated_ptr_idx);    
}
