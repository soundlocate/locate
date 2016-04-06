#ifndef _POSITIONBUFFER_H
#define _POSITIONBUFFER_H

#include <vector>
#include <chrono>

#include "util/types.h"
#include "Position.h"
#include "PositionCache.h"

class PositionBuffer {
public:
	PositionBuffer(u64 meanWindow, u64 maxKeep);

	u64 add(Position p);

	u64 deleteOlderThan(std::chrono::high_resolution_clock::duration d);

	PositionCache getPositions();
private:
	u64 meanWindow;
	u64 maxKeep;

	std::vector<FrequencyCache> data;
};

#endif
