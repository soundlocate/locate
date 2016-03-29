#ifndef _ALGORITHM_H
#define _ALGORITHM_H

#include <vector>

#include "../Microfone.h"
#include "../util/types.h"
#include "../FFTPacket.h"

class Algorithm {
public:
	virtual v3 locate(std::vector<Microfone> mics, FFTPacket p, v3 center) = 0;
};

#endif
