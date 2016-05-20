#include "DissimilarityMeanDist.h"

DissimilarityMeanDist::DissimilarityMeanDist(f64 maxClusterSize)
    : maxClusterSize(maxClusterSize) {}

f64 DissimilarityMeanDist::dissimilarity(const Cluster & a, const Cluster & b) {
    f64 ret = 0;
    f64 tmp;

    for(auto posa : a.objs) {
        for(auto posb : b.objs) {
            tmp = (posa.pos - posb.pos).norm();
            ret += tmp * tmp;
        }
    }

    ret = sqrt(ret / (a.objs.size() * b.objs.size()));

    return ret;
}

u64 DissimilarityMeanDist::shouldMerge(const f64 &     dissimilarity,
                                       const Cluster & a,
                                       const Cluster & b) const {
    f64 maximumSmallestDist = 0;
    f64 smallestDist        = std::numeric_limits<f64>::max();
    f64 dist;

    for(auto posb : b.objs) {
        for(auto posa : a.objs) {
            dist = (posa.pos - posb.pos).norm();

            if(dist < smallestDist) smallestDist = dist;
        }

        if(smallestDist > maximumSmallestDist)
            maximumSmallestDist = smallestDist;
    }

    return maximumSmallestDist < maxClusterSize;
}

Position DissimilarityMeanDist::center(const Cluster & a) const {
    return a.center();
}
