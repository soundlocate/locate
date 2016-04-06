#ifndef _POSITIONCACHE_H
#define _POSITIONCACHE_H

#include <exception>
#include <vector>

#include "util/types.h"
#include "Position.h"
#include "FrequencyCache.h"

class PositionCache {
public:
	PositionCache(std::vector<FrequencyCache> & data);

	u64 size();

	Position& nextPosition();

    u64 rewind();
private:
	u64 fpos = 0;
	u64 ppos = 0;

	std::vector<FrequencyCache> data;
};

#endif
