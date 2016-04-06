#ifndef _LOCATOR3D_H
#define _LOCATOR3D_H

#include <vector>
#include <array>
#include <limits>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <cassert>

#include "util/constant.h"
#include "util/types.h"
#include "Microfone.h"
#include "FFTPacket.h"
#include "algorithms/Algorithm.h"
#include "PositionRater.h"

class Locator3D {
public:
	Locator3D(std::vector<Microfone> microfones, std::vector<Algorithm *> a);

	v3 locate(FFTPacket packet);
private:
	u64 numMics;
  
	std::vector<Microfone> mics;
	std::vector<Algorithm *> algorithms;
	PositionRater rater;

	v3 center;
	f64 maxFreq;

	f64 f64Max = std::numeric_limits<f64>::max();
};

#endif
