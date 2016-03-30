#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <vector>
#include "util/types.h"

template<typename T>
class RingBuffer {
public:
	RingBuffer(u64 size) : m_size(size) {
		buffer.reserve(size);
	}

	int add(T item) {
		if(index < m_size)
			buffer.push_back(item);
		else
			buffer[index++ % m_size] = item;

		return 0;
	}

	std::vector<T> getItems() {
		return buffer;
	}

private:
	u64 index = 0;
	u64 m_size;

	std::vector<T> buffer;
};

#endif
