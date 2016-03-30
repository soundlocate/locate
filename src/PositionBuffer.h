#ifndef _POSITIONBUFFER_H
#define _POSITIONBUFFER_H

#include <vector>

#include "util/types.h"
#include "Position.h"
#include "PositionCache.h"

class PositionBuffer {
public:
	PositionBuffer(u64 meanWindow, u64 maxKeep) : meanWindow(meanWindow), maxKeep(maxKeep) {}

	int add(Position p) {
		// search for existing frequency cache
		for(auto fcache : data) {
			if(fcache.getFreq() == p.frequency) {
				std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
						  << "adding to existing frequency cache:" << p.pos
						  << ", freq: " << fcache.getFreq() << std::endl;
				fcache.add(p);
				return 0;
			}
		}

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "adding to new frequency cache:" << p.pos
				  << ", freq: " << p.frequency << std::endl;

		// no frequency cache exists, create a new one
	    data.push_back(FrequencyCache(p.frequency, meanWindow, maxKeep));
		data[data.size() - 1].add(p);

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
