#ifndef _POSITIONCACHE_H
#define _POSITIONCACHE_H

#include <exception>
#include <vector>

#include "util/types.h"
#include "Position.h"
#include "FrequencyCache.h"

class IndexOutOfBoundsException : public std::exception {
	virtual const char* what() const noexcept override
	{
		return "Index out of bounds";
	}
} oobex;

class PositionCache {
public:
	PositionCache(std::vector<FrequencyCache> & data) : data(data) {}

	u64 size() {
		u64 size = 0;
		for(auto fcache : data)
			size += fcache.getPositions().size();

		return size;
	}

	Position nextPosition() {
		if(ppos > (data.size() - 1))
			throw oobex;

		if(fpos >= (data[ppos].getPositions().size())) {
			fpos = 0;

			do {
				ppos++;

				if(ppos >= data.size())
					throw oobex;
			} while(data[ppos].getPositions().size() == 0);
		}

	    Position pos;

		pos = data[ppos].getPositions()[fpos];

		fpos++;

		return pos;
	}

    u64 rewind() {
		ppos = 0;
		fpos = 0;

		return 0;
	}
private:
	u64 fpos = 0;
	u64 ppos = 0;

	std::vector<FrequencyCache> data;
};

#endif
