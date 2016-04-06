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
 Locator3D(std::vector<Microfone> microfones, std::vector<Algorithm *> a) : numMics(microfones.size()), mics(microfones), algorithms(a), rater(mics) {

		assert(mics.size() == numMics);

		std::cout << "numMics: " << numMics << std::endl;

        // calculate maximum frequency
	    f64 maxDist = std::numeric_limits<double>::min();
	    f64 curDist = 0;

		for(u64 i = 0; i < numMics; i++) {
			for(u64 j = i + 1; j < numMics; j++) {
				curDist = (microfones[i].pos - microfones[j].pos).norm();
				maxDist = curDist > maxDist ? curDist : maxDist;
			}
		}

		// f = c / lambda
		maxFreq =  sound::speedInAir / maxDist;

		std::cout << "maxFreq: " << maxFreq << std::endl;

		// calculate geometrical center of all microfones
		for(u64 i = 0; i < numMics; i++) {
			center += microfones[i].pos;
		}

		center /= numMics;

		std::cout << "center: " << center << std::endl;
	}

	// test values (2, 2, 2)
	/*
	  freq = 171.15;
	  x[0] = 2 * std::sqrt(3) - 2;
	  x[1] = 1;
	  x[2] = 0.7452;
	  x[3] = 0.565017;
	*/

	// locate a packet
	v3 locate(FFTPacket packet) {
		std::vector<v3> positions;

		assert(packet.getSineCount() == numMics);

		if(packet.sines[0].freq > maxFreq)
			return v3(f64Max, f64Max, f64Max);

		for(auto a : algorithms) {
			positions.push_back(a->locate(mics, packet, center));
		}

		// std::cout << packet.sines[0].freq << ":" << rater.bestPosition(positions, packet) << std::endl;

		return rater.bestPosition(positions, packet);
	};
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
