#ifndef _FREQUENCYCACHE_H
#define _FREQUENCYCACHE_H

#include <chrono>
#include <vector>

#include "util/types.h"
#include "Position.h"
#include "RingBuffer.h"

class FrequencyCache {
public:
	FrequencyCache(f64 frequency, u64 meanWindow, u64 maxKeep);

	int add(Position p);

	u64 deleteOlderThan(std::chrono::high_resolution_clock::duration d);

	std::vector<Position> & getPositions();

	f64 getFreq();
private:
	f64 freq;

	u64 meanWindow;
	u64 maxKeep;

	RingBuffer<std::chrono::time_point<std::chrono::high_resolution_clock>> timestamps;
	RingBuffer<Position> lastPositions;
	RingBuffer<Position> positions;
};

#endif
