#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <cassert>

#include <vector>

#include "util/types.h"

template <typename T>
class RingBuffer {
public:
    RingBuffer(u64 size) : m_size(size) { buffer.reserve(size); }

    u64 add(T item) {
        if(buffer.size() < m_size) {
            buffer.push_back(item);
            index++;
        } else
            buffer[index++ % m_size] = item;

        return 0;
    }

    std::vector<T> & getItems() { return buffer; }

    u64 remove(u64 index) {
        assert(index < buffer.size());

        if(index > buffer.size())
            return (u64)-1;
        else
            buffer.erase(buffer.begin() + index);

        return 0;
    }

private:
    u64 index = 0;
    u64 m_size;

    std::vector<T> buffer;
};

#endif
