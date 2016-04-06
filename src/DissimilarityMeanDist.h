#ifndef _DISSIMILARITYMEANDIST_H
#define _DISSIMILARITYMEANDIST_H

#include "Dissimilarity.h"
#include "Position.h"

class DissimilarityMeanDist : public Dissimilarity {
public:
	DissimilarityMeanDist(f64 maxClusterSize) : maxClusterSize(maxClusterSize) {}

	f64 dissimilarity(const Cluster & a, const Cluster & b) override {
		f64 ret = 0;
		f64 tmp;

		for(auto posa : a.objs) {
			for(auto posb : b.objs) {
				tmp = (posa.pos - posb.pos).norm();
				ret += tmp * tmp;
			}
		}

		ret =  sqrt(ret / (a.objs.size() * b.objs.size()));

		return ret;
	}

	u64 shouldMerge(const f64 & dissimilarity, const Cluster & a, const Cluster & b) const override {
		f64	maximumSmallestDist = 0;
		f64 smallestDist = std::numeric_limits<f64>::max();
		f64 dist;

		for(auto posb : b.objs) {
			for(auto posa : a.objs) {
				dist = (posa.pos - posb.pos).norm();

				if(dist < smallestDist)
					smallestDist = dist;
			}

			if(smallestDist > maximumSmallestDist)
				maximumSmallestDist = smallestDist;
		}

	    return maximumSmallestDist < maxClusterSize;
	}
	Position center(const Cluster & a) const {
		return a.center();
	}


private:
	f64 maxClusterSize;
};

#endif
