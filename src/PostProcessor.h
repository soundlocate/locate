#ifndef _POSTPROCESSOR_H
#define _POSTPROCESSOR_H

#include <vector>

#include "FFTPacket.h"
#include "util/types.h"
#include "Microfone.h"
#include "PositionBuffer.h"
#include "Clusterer.h"

class PostProcessor {
public:
	PostProcessor(std::vector<Microfone> mics, f64 maxClusterSize, f64 maxDist, u64 meanWindow, u64 maxKeep) :
		clusterer(maxClusterSize), positionBuffer(meanWindow, maxKeep), mics(mics),
		maxDist(maxDist), meanWindow(meanWindow), maxKeep(maxKeep) {}

	u64 add(FFTPacket p, v3 pos) {
		if(pos.norm() > maxDist
		   || std::isnan(pos.x) || std::isnan(pos.y) || std::isnan(pos.z)
		   || std::isinf(pos.x) || std::isinf(pos.y) || std::isinf(pos.z))
			return 1;

		Position position;
		position.frequency = p.sines[0].freq;
	    position.amplitude = p.meanAmplitude();
		position.pos = pos;

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: " << "adding position: "
				  << position.frequency << ", "
				  << position.amplitude << ", "
				  << position.pos << std::endl;

		positionBuffer.add(position);

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "new size: " << positionBuffer.getPositions().size() << std::endl;
		// recluster all positions
		if(positionBuffer.getPositions().size() % (meanWindow * maxKeep)) {
			positions =	clusterer.cluster(positionBuffer.getPositions());
		}

		return 0;
	}

	std::vector<Position> & getPositions() {
		return positions;
	}

private:
	Clusterer clusterer;
	PositionBuffer positionBuffer;
	std::vector<Microfone> mics;
	std::vector<Position> positions;

	f64 maxDist;

	u64 meanWindow;
	u64 maxKeep;
};

#endif
