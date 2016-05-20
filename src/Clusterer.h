#ifndef _CLUSTERER_H
#define _CLUSTERER_H

#include <deque>
#include <iostream>
#include <limits>
#include <vector>

#include <cassert>

#include "Cluster.h"
#include "Dissimilarity.h"
#include "Position.h"
#include "PositionCache.h"
#include "util/types.h"

class Clusterer {
public:
    Clusterer(Dissimilarity * d);

    std::vector<Position> cluster(PositionCache positions);

private:
    Dissimilarity * dissimilarity;

    std::deque<Cluster>   pos;
    std::vector<Position> clusters;
};

#endif
