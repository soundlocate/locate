#include "PositionCache.h"

class IndexOutOfBoundsException : public std::exception {
	virtual const char* what() const noexcept override
		{
			return "Index out of bounds";
		}
} oobex;

PositionCache::PositionCache(std::vector<FrequencyCache> & data) : data(data) {}

u64 PositionCache::size() {
	u64 size = 0;
	for(auto fcache : data) {
		size += fcache.getPositions().size();
	}

	return size;
}

Position & PositionCache::nextPosition() {
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

	fpos++;

	return data[ppos].getPositions()[fpos];
}

u64 PositionCache::rewind() {
	ppos = 0;
	fpos = 0;

	return 0;
}
