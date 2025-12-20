#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>

#define CACHE_LINE 64
#define CAPACITY 1024             
#define MASK (CAPACITY - 1)

struct data {
    uint64_t value;
};

struct ring_buffer {
    struct data data[CAPACITY];
};

struct padded_atomic_size {
    atomic_size_t value;
    char padding[CACHE_LINE - sizeof(atomic_size_t)];
};

struct spsc_queue {
    struct ring_buffer buffer;

    struct padded_atomic_size head;

    struct padded_atomic_size tail;
};

void spsc_init(struct spsc_queue *q) {
    atomic_init(&q->head.value, 0);
    atomic_init(&q->tail.value, 0);
}