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

int spsc_push(struct spsc_queue *q, struct item item) {
    size_t tail = atomic_load_explicit(&q->tail.value, memory_order_relaxed);

    size_t next_tail = (tail + 1) & MASK;

    if (next_tail == atomic_load_explicit(&q->head.value, memory_order_acquire)) {
        return 0;
    }

    q->buffer.data[tail] = item;

    atomic_store_explicit(&q->tail.value, next_tail, memory_order_release);
    return 1;
}

int spsc_pop(struct spsc_queue *q, struct item *out) {
    size_t head = atomic_load_explicit(&q->head.value, memory_order_relaxed);

    if (head == atomic_load_explicit(&q->tail.value, memory_order_acquire)) {
        return 0;
    }

    *out = q->buffer.data[head];

    atomic_store_explicit(&q->head.value, (head + 1) & MASK, memory_order_release);
    return 1;
}

//same as single producer, single consumer
int spmc_push(struct spsc_queue *q, struct item item) {
    size_t tail = atomic_load_explicit(&q->tail.value, memory_order_relaxed);

    size_t next_tail = (tail + 1) & MASK;

    if (next_tail == atomic_load_explicit(&q->head.value, memory_order_acquire)) {
        return 0;
    }

    q->buffer.data[tail] = item;

    atomic_store_explicit(&q->tail.value, next_tail, memory_order_release);
    return 1;
}

int spmc_pop(struct spsc_queue *q, struct item *out) {
    size_t head;

    while (1) {
        head = atomic_load_explicit(&q->head.value, memory_order_relaxed);

        size_t tail = atomic_load_explicit(&q->tail.value, memory_order_acquire);

        if (head == tail) {
            return 0;
        }

        size_t next_head = (head + 1) & MASK;

        if (atomic_compare_exchange_weak_explicit(&q->head.value, &head, next_head, memory_order_acquire, memory_order_relaxed)) {
            break;
        }
    }
    *out = q->buffer.data[head];
    return 1;
}

int main(void){
    return 0;
}