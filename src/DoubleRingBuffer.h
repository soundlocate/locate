#ifndef _DOUBLERINGBUFFER_H
#define _DOUBLERINGBUFFER_H

#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>

#include "util/types.h"

// ToDo(robin): implement the big five
// ToDo(robin): is there something that can be done to prevent data currently
// processed getting overwritten?

// A ringbuffer for dynamic size arrays
template <typename T>
class DoubleRingBuffer {
private:
    T *                     buffer;
    volatile u64            pushIndex = 0;
    volatile u64            popIndex  = 0;
    u64                     size;
    u64                     arraySize;
    u64                     typeSize;
    std::mutex              lock;
    std::mutex              popLock;
    std::condition_variable elemAvail;

public:
    DoubleRingBuffer() {}

    DoubleRingBuffer(u64 arraySize, u64 size)
        : size(size), arraySize(arraySize) {
        typeSize = sizeof(T) * arraySize;
        buffer   = (T *)malloc(typeSize * size);
    }

    // pushes an element on the buffer
    u64 push(const T & value) {
        std::memcpy(buffer[(pushIndex % size) * arraySize], &value, typeSize);
        pushIndex++;

        elemAvail.notify_one();
        return 0;
    }

    // returns an pointer to the where the next element will be read
    T * pushPtr() { return &buffer[(pushIndex % size) * arraySize]; }

    // marks that the data has been read and the push index should increment
    void pushDone() {
        pushIndex++;
        elemAvail.notify_one();
    }

    T * popPtr() {
        std::lock_guard<std::mutex> lock_guard(popLock);

        std::unique_lock<std::mutex> lk(lock);

        while(pushIndex <= popIndex) elemAvail.wait(lk);

        return &buffer[(popIndex++ % size) * arraySize];
    }

    // gets one element from the buffer, block until one is available
    T pop() {
        // Note(robin): on x86 native size reads are atomic

        std::lock_guard<std::mutex> lock_guard(popLock);

        // wait for data
        std::unique_lock<std::mutex> lk(lock);
        while(pushIndex <= popIndex) elemAvail.wait(lk);

        return buffer[(popIndex++ % size) * arraySize];
    }

    ~DoubleRingBuffer() { free(buffer); }
};

#endif
