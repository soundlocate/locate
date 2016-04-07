#ifndef _POSITIONRATER_H
#define _POSITIONRATER_H

#include <vector>

#include "FFTPacket.h"
#include "util/types.h"
#include "Microfone.h"

class PositionRater {
public:
	PositionRater(std::vector<Microfone> m);

	// ToDo(robin): actually use the best position (-> use correct vs. located phase)
	v3 bestPosition(std::vector<v3> & positions, FFTPacket::Sinus * p);
private:
	std::vector<Microfone> mics;
};

#endif
