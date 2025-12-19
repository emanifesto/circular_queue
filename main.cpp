#include <cstddef>
#include <cstdint>
#include <memory>

struct Data {
    uint64_t value;
};


template <typename T>
struct RingBuffer {
    const size_t capacity;
    std::unique_ptr<T[]> buffer;

    explicit RingBuffer(size_t cap)
        : capacity(cap),
          buffer(std::make_unique<T[]>(cap)) {}
};
