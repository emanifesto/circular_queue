#include <stddef.h>
#include <stdint.h>

#define CACHE_LINE 64
#define CAPACITY 1024             
#define MASK (CAPACITY - 1)

struct data {
    uint64_t value;
};

struct ring_buffer {
    struct data data[CAPACITY];
};
