#ifndef _CLUSTERER_H
#define _CLUSTERER_H

#include <vector>
#include <deque>
#include <limits>
#include <iostream>

#include "util/types.h"
#include "Position.h"
#include "PositionCache.h"
#include "Cluster.h"
#include "Dissimilarity.h"

class Clusterer {
public:
	Clusterer(Dissimilarity * d) : dissimilarity(d) {
		assert(d != nullptr);
	}

	std::vector<Position> cluster(PositionCache positions) {
	    clusters.clear();
		pos.clear();

		for(u64 i = 0; i < positions.size(); i++) {
			pos.push_back(Cluster(positions.nextPosition()));
		}

	    u64 other = 0;
		f64 dsim = std::numeric_limits<f64>::max();
		f64 dsimTmp;
		u64 hasMerged;
		u64 mergeCanidateFound;

		do {
			hasMerged = false;
			mergeCanidateFound = false;

			for(u64 i = 0; i < pos.size(); i++) {
				// find cluster with minimum dissimiliarity in respect to i
				for(u64 j = 0; j < pos.size(); j++) {
					// do not merge the clusters with itself
					if(i == j) continue;

					dsimTmp = dissimilarity->dissimilarity(pos[i], pos[j]);

					if(dsimTmp < dsim) {
						dsim = dsimTmp;
						other = j;

						mergeCanidateFound = true;
					}
				}

				// merge i with the cluster with lowest dissimilarity
				if(mergeCanidateFound && dissimilarity->shouldMerge(dsim, pos[i], pos[other])) {
					pos[i].merge(pos[other]);

					pos.erase(pos.begin() + other);

					hasMerged = true;
				}

				dsim = std::numeric_limits<f64>::max();
				other = 0;
			}
		} while(hasMerged); /* try and merge clusters until no cluster gets merged anymore */

		for(auto cluster : pos) {
			clusters.push_back(dissimilarity->center(cluster));
		}

		return clusters;
	}
private:
	Dissimilarity * dissimilarity;

	std::deque<Cluster> pos;
	std::vector<Position> clusters;
};

#endif
