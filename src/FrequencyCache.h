#ifndef _FREQUENCYCACHE_H
#define _FREQUENCYCACHE_H

#include <vector>

#include "util/types.h"
#include "Position.h"
#include "RingBuffer.h"

class FrequencyCache {
public:
	FrequencyCache(f64 frequency, u64 meanWindow, u64 maxKeep) : freq(frequency), meanWindow(meanWindow), maxKeep(maxKeep), lastPositions(meanWindow), positions(maxKeep) {
		Position pos;
		pos.frequency = freq;
		pos.amplitude = 0;
		pos.pos = v3();
	}

	// ToDo(robin): wrong!!! (maybe)
	int add(Position p) {
		Position pos;

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "got position: " << p.pos << std::endl;

		pos.frequency = freq;
		pos.amplitude = 0;
		pos.pos = v3();

		// add this position to the last positions
		lastPositions.add(p);

		// calculate mean position of the last positions
		for(auto position : lastPositions.getItems()) {
			pos.amplitude += position.amplitude;
			pos.pos += position.pos;
		}

		pos.amplitude /= meanWindow;
		pos.pos /= meanWindow;

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "adding position: " << pos.pos << std::endl;

		positions.add(pos);

		return 0;
	}

	std::vector<Position> getPositions() {
		return positions.getItems();
	}

	f64 getFreq() {
		return freq;
	}
private:
	f64 freq;

	u64 meanWindow;
	u64 maxKeep;

	RingBuffer<Position> lastPositions;
	RingBuffer<Position> positions;
};

#endif
