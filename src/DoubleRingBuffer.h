#ifndef _DOUBLERINGBUFFER_H
#define _DOUBLERINGBUFFER_H

#include <cstring>

#include "util/types.h"

// ToDo(robin): implement the big five
// A ringbuffer for dynamic size arrays
template<typename T>
class DoubleRingBuffer {
private:
	T * buffer;
	volatile u64 pushIndex = 0;
	volatile u64 popIndex = 0;
	u64 size;
	u64 arraySize;
	u64 typeSize;

public:
	DoubleRingBuffer() {}

	DoubleRingBuffer(u64 arraySize, u64 size) : size(size), arraySize(arraySize) {
		typeSize = sizeof(T) * arraySize;
		buffer = (T*) malloc(typeSize * size);
	}

	// pushes an element on the buffer
	u64 push(const T & value) {
		std::memcpy(buffer[(pushIndex % size) * arraySize], &value, typeSize);
		pushIndex++;

		return 0;
	}

	// returns an pointer to the where the next element will be read
	T* pushPtr() {
		return &buffer[(pushIndex % size) * arraySize];
	}

	// marks that the data has been read and the push index should increment
	void pushDone() {
		pushIndex++;
	}

	T* popPtr() {
		while(pushIndex <= popIndex) {}

		return &buffer[(popIndex++ % size) * arraySize];
	}

	// gets one element from the buffer, block until one is available
	T pop() {
		// Note(robin): on x86 native size reads are atomic

		// wait for data
		while(pushIndex <= popIndex) {}

		return buffer[(popIndex++ % size) * arraySize];
	}

	~DoubleRingBuffer() {
		free(buffer);
	}
};

#endif
