#ifndef _PHASEONLY_H
#define _PHASEONLY_H

#include <unordered_map>
#include <limits>
#include <cassert>
#include <iostream>

#include "/opt/intel/mkl/include/mkl_lapacke.h"

#include "../util/constant.h"
#include "../util/math.h"
#include "Algorithm.h"

class PhaseOnly : public Algorithm {
public:
	PhaseOnly(u64 numMics, f64 accuracy) : numMics(numMics), accuracy(accuracy) {
		target = (f64 *) malloc(sizeof(f64) * (numMics - 1));
		singular = (f64 *) malloc(sizeof(f64) * (numMics - 1));
		matrix = (f64 *) malloc(sizeof(f64) * (3 * numMics));
		x = (f64 *) malloc(sizeof(f64) * numMics);
		r = (f64 *) malloc(sizeof(f64) * numMics);
	}

	v3 locate(std::vector<Microfone> mics, FFTPacket p, v3 center) override {
		f64 freq = p.sines[0].freq;
		v3 d(f64Max, f64Max, f64Max);
		v3 pos;
	    u64 nitr = 0;

		// calculate path differences
		for(u64 i = 0; i < numMics; i++) {
			x[i] = math::deltaPhaseToDistance(p.sines[i].phase, p.sines[0].phase, freq);
		}

		// use last position of this frequency, or if new freqency start at center
		auto lastPos = lastPositions.find(freq);
		if(lastPos != lastPositions.end()) {
			pos = lastPos->second;
		} else {
			pos = center;
		}

		// solve the system of equations iterativly
		while(d.norm() > accuracy) {
			// calculate distance between mics and the current guess
			for(u64 i = 0; i < numMics; i++) {
				r[i] = (mics[i].pos - pos).norm();
			}

			// fill the matrix
			for (u64 i = 0; i < numMics - 1; i++) {
				matrix[3 * i + 0] = ((pos.x - mics[i + 1].pos.x) / r[i + 1]) - ((pos.x - mics[0].pos.x) / r[0]);
				matrix[3 * i + 1] = ((pos.y - mics[i + 1].pos.y) / r[i + 1]) - ((pos.y - mics[0].pos.y) / r[0]);
				matrix[3 * i + 2] = ((pos.z - mics[i + 1].pos.z) / r[i + 1]) - ((pos.z - mics[0].pos.z) / r[0]);
		    }

			// fill the vector
			for (u64 i = 0; i < numMics - 1; i++) {
				target[i] = x[i + 1] - r[i + 1] + r[0];
			}

			// solve least squares
		    LAPACKE_dgelsd(LAPACK_ROW_MAJOR, numMics - 1, 3, 1, matrix, 3, target, 1, singular, -1, &rank);

			// get dx, dy and dz
			d = v3(target[0], target[1], target[2]);

			// iterate position
			pos += d;

			// it just does not converge
			nitr++;
			if(nitr > 50) {
//				std::cerr << "did not converge for freq: " << freq << std::endl;

				// lets not save that one, it is totally wrong, lets also reset the lastPosition to center
				lastPositions[freq] = center;
				return pos;
			}
		}
		// save this position as starting point for next time (if sensible value)
		if(pos.norm() < locate::maxDist)
			lastPositions[freq] = pos;
		else
			lastPositions[freq] = center;

		return pos;
	}

	~PhaseOnly() {
		free(target);
		free(singular);
		free(matrix);
		free(x);
		free(r);
	}
private:
	std::unordered_map<f64, v3> lastPositions;

	u64 numMics;
	i32 rank;

	f64 accuracy = 0.01;
	f64 f64Max = std::numeric_limits<f64>::max();

	f64 * target;
	f64 * singular;
	f64 * matrix;
 	f64 * x;
	f64 * r;
};

#endif
