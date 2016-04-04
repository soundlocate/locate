#ifndef _FREQUENCYCACHE_H
#define _FREQUENCYCACHE_H

#include <chrono>
#include <vector>

#include "util/types.h"
#include "Position.h"
#include "RingBuffer.h"

class FrequencyCache {
public:
	FrequencyCache(f64 frequency, u64 meanWindow, u64 maxKeep) : freq(frequency), meanWindow(meanWindow), maxKeep(maxKeep), timestamps(maxKeep), lastPositions(meanWindow), positions(maxKeep) {
		Position pos;
		pos.frequency = freq;
		pos.amplitude = 0;
		pos.pos = v3();
	}

	// ToDo(robin): wrong!!! (maybe)
	int add(Position p) {
		Position pos;

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

		positions.add(pos);
		timestamps.add(std::chrono::high_resolution_clock::now());

		return 0;
	}

	u64 deleteOlderThan(std::chrono::high_resolution_clock::duration d) {
		auto now = std::chrono::high_resolution_clock::now();
		auto timeStamps = timestamps.getItems();

		for(u64 i = 0; i < timeStamps.size(); i++) {
			if(now - timeStamps[i] > d) {
				timestamps.remove(i);
				positions.remove(i);
			}
		}

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

	RingBuffer<std::chrono::time_point<std::chrono::high_resolution_clock>> timestamps;
	RingBuffer<Position> lastPositions;
	RingBuffer<Position> positions;
};

#endif
