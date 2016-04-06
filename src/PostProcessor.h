#ifndef _POSTPROCESSOR_H
#define _POSTPROCESSOR_H

#include <vector>
#include <chrono>

#include "FFTPacket.h"
#include "util/types.h"
#include "Microfone.h"
#include "PositionBuffer.h"
#include "Clusterer.h"
#include "Dissimilarity.h"
#include "DissimilarityMeanDist.h"
#include "DissimilarityMeanDirection.h"

class PostProcessor {
public:
	// Note(robin): only valid values for dissimilarity are allowed (), no validity checks performed!
	PostProcessor(std::vector<Microfone> mics, f64 maxClusterSize, f64 maxDist,
				  u64 dissimilarity, u64 meanWindow, u64 maxKeep, f64 keepTime) :
		clusterer(dissimilarity == 0 ?
				  (Dissimilarity *) new DissimilarityMeanDirection(mics, maxClusterSize) :
				  (Dissimilarity *) new DissimilarityMeanDist(maxClusterSize)),
		positionBuffer(meanWindow, maxKeep), mics(mics), maxDist(maxDist),
		meanWindow(meanWindow), maxKeep(maxKeep), keepTime(keepTime) {}

	u64 add(FFTPacket p, v3 pos) {
		if(pos.norm() > maxDist
		   || std::isnan(pos.x) || std::isnan(pos.y) || std::isnan(pos.z)
		   || std::isinf(pos.x) || std::isinf(pos.y) || std::isinf(pos.z))
			return 1;

		Position position;
		position.frequency = p.sines[0].freq;
	    position.amplitude = p.meanAmplitude();
		position.pos = pos;

		positionBuffer.add(position);

		// recluster all positions
		if(counter++ % 10 == 0) {
			positionBuffer.deleteOlderThan(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::milliseconds((int) (keepTime * 1000))));

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
	u64 counter;

	f64 keepTime;
};

#endif
