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
	PostProcessor(const std::vector<Microfone> mics, const f64 maxClusterSize, const f64 maxDist,
				  const u64 dissimilarity, const u64 meanWindow, const u64 maxKeep,
				  const f64 keepTime);

	u64 add(FFTPacket p, v3 pos);

	std::vector<Position> & getPositions();
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
