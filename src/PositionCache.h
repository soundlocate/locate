#ifndef _POSITIONCACHE_H
#define _POSITIONCACHE_H

#include <exception>
#include <vector>

#include "FrequencyCache.h"
#include "Position.h"
#include "util/types.h"

class PositionCache {
public:
    PositionCache(std::vector<FrequencyCache> & data);

    u64 size();

    Position nextPosition();

    u64 rewind();

private:
    u64 fpos   = 0;
    u64 ppos   = 0;
    u64 m_size = 0;

    std::vector<FrequencyCache> data;
};

#endif
