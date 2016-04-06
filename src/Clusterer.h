#ifndef _CLUSTERER_H
#define _CLUSTERER_H

#include <vector>
#include <deque>
#include <limits>
#include <iostream>

#include <cassert>

#include "util/types.h"
#include "Position.h"
#include "PositionCache.h"
#include "Cluster.h"
#include "Dissimilarity.h"

class Clusterer {
public:
	Clusterer(Dissimilarity * d);

	std::vector<Position> cluster(PositionCache positions);
private:
	Dissimilarity * dissimilarity;

	std::deque<Cluster> pos;
	std::vector<Position> clusters;
};

#endif
