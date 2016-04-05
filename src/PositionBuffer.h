#ifndef _POSITIONBUFFER_H
#define _POSITIONBUFFER_H

#include <vector>
#include <chrono>

#include "util/types.h"
#include "Position.h"
#include "PositionCache.h"

class PositionBuffer {
public:
	PositionBuffer(u64 meanWindow, u64 maxKeep) : meanWindow(meanWindow), maxKeep(maxKeep) {}

	u64 add(Position p) {
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

	u64 deleteOlderThan(std::chrono::high_resolution_clock::duration d) {
		for(auto & fcache : data) {
			fcache.deleteOlderThan(d);
		}

		return 0;
	}

	PositionCache getPositions() {
		return PositionCache(data);
	}
private:
	u64 meanWindow;
	u64 maxKeep;

	std::vector<FrequencyCache> data;
};

#endif
