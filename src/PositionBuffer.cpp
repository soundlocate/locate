#include "PositionBuffer.h"

PositionBuffer::PositionBuffer(u64 meanWindow, u64 maxKeep) :
	meanWindow(meanWindow), maxKeep(maxKeep) {}

u64 PositionBuffer::add(Position p) {
	// search for existing frequency cache

	for(auto & fcache : data) {
		if(fcache.getFreq() == p.frequency) {
			fcache.add(p);
			return 0;
		}
	}

	// no frequency cache exists, create a new one
	data.push_back(FrequencyCache(p.frequency, meanWindow, maxKeep));
	data[data.size() - 1].add(p);

	return 0;
}

u64 PositionBuffer::deleteOlderThan(std::chrono::high_resolution_clock::duration d) {
	for(auto & fcache : data) {
		fcache.deleteOlderThan(d);
	}

	return 0;
}

PositionCache PositionBuffer::getPositions() {
	return PositionCache(data);
}
