#ifndef _LOCATOR3D_H
#define _LOCATOR3D_H

#include <vector>
#include <array>
#include <limits>
#include <unordered_map>
#include <cassert>

#include "/opt/intel/mkl/include/mkl_lapacke.h"


#include "util/constant.h"
#include "util/types.h"
#include "Microfone.h"
#include "FFTPacket.h"

template<int numMics>
class Locator3D {
public:
	Locator3D(std::array<Microfone, numMics> microfones): mics(microfones) {
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

	v3 locate(FFTPacket packet) {
	    f64 freq = packet.sines[0].freq;
		v3 d(f64Max, f64Max, f64Max);
		v3 pos;
	    u64 nitr = 0;

//		std::cout << "freq: " << freq << std::endl;

		assert(packet.getSineCount() == numMics);

		// use last position of this frequency, or if new freqency start at center
		auto lastPos = lastPositions.find(freq);
		if(lastPos != lastPositions.end()) {
			pos = lastPos->second;
		} else {
			pos = center;
		}

//		std::cout << "initial guess: " << pos << std::endl;

		// calculate path differences
		for(u64 i = 0; i < numMics; i++) {
			x[i] = deltaPhaseToDistance(packet.sines[i].phase, packet.sines[0].phase, freq);
//			std::cout << "x[" << i << "] = " << x[i] << std::endl;
		}

		// test values (2, 2, 2)
		/*
		freq = 171.15;
		x[0] = 2 * std::sqrt(3) - 2;
		x[1] = 1;
		x[2] = 0.7452;
		x[3] = 0.565017;
		*/

		while(d.norm() > accuracy) {
			// calculate distance between mics and the current guess
			for(u64 i = 0; i < numMics; i++) {
				r[i] = (mics[i].pos - pos).norm();
			}

			// fill the matrix
			for (int i = 0; i < numMics; i++) {
				matrix[4 * i + 0] = (pos.x - mics[i].pos.x) / r[i];
				matrix[4 * i + 1] = (pos.y - mics[i].pos.y) / r[i];
				matrix[4 * i + 2] = (pos.z - mics[i].pos.z) / r[i];
				matrix[4 * i + 3] = -1;
			}

			// fill the vector
			for (int i = 0; i < numMics; i++) {
				target[i] = x[i] - r[i];
			}

/* Debug output
			for(int i = 0; i < numMics; i++) {
				std::cout << matrix[4 * i + 0] << "\t"
						  << matrix[4 * i + 1] << "\t"
						  << matrix[4 * i + 2] << "\t"
						  << matrix[4 * i + 3] << "\t\t"
						  << target[i] << std::endl;
			}
*/

			// solve least squares
		    LAPACKE_dgelsd(LAPACK_ROW_MAJOR, numMics, 4, 1, matrix, numMics, target, 1, singular, -1, &rank);

			// get dx, dy and dz
			d = v3(target[0], target[1], target[2]);

			// iterate position
			pos += d;

//			std::cout << "d: " << d << std::endl;
//			std::cout << "pos: " << pos << std::endl;
//			std::cout << "nitr: " << nitr << std::endl;

			// it just does not converge
			nitr++;
			if(nitr > 50) {
				std::cerr << "did not converge for freq: " << freq << std::endl;

				// lets not save that one, it is totally wrong, lets also reset the lastPosition to center
				lastPositions[freq] = center;
				return pos;
			}
		}

		// save this position as starting point for next time
		lastPositions[freq] = pos;

		return pos;
	};
private:
	f64 deltaPhaseToDistance(f64 a, f64 b, f64 freq) {
	    f64 diff = b - a;

		if(diff < -math::PI) {
			diff += 2 * math::PI;
		} else if(diff > math::PI) {
			diff -= 2 * math::PI;
		}

		diff *= sound::speedInAir;
		diff /= 2 * math::PI * freq;

		return diff;
	}

	f64 accuracy = 0.001;

	std::array<Microfone, numMics> mics;
	std::unordered_map<f64, v3> lastPositions;
	v3 center;
	f64 maxFreq;

	f64 f64Max = std::numeric_limits<f64>::max();
	f64 target[4];
	f64 singular[4];
	f64 matrix[4 * numMics];
	i32 rank;
	f64 x[numMics];
	f64 r[numMics];
};

#endif
