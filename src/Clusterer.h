#ifndef _CLUSTERER_H
#define _CLUSTERER_H

#include <vector>
#include <deque>
#include <limits>
#include <iostream>

#include "util/types.h"
#include "Position.h"
#include "PositionCache.h"

class Clusterer {
public:
	Clusterer(f64 maxClusterSize) : maxClusterSize(maxClusterSize) {}

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

					dsimTmp = dissimilarity(pos[i], pos[j]);

					if(dsimTmp < dsim) {
						dsim = dsimTmp;
						other = j;

						mergeCanidateFound = true;
					}
				}

				// merge i with the cluster with lowest dissimilarity
				if(mergeCanidateFound && shouldMerge(pos[i], pos[other])) {
					pos[i].merge(pos[other]);

					pos.erase(pos.begin() + other);

					hasMerged = true;
				}

				dsim = std::numeric_limits<f64>::max();
				other = 0;
			}
		} while(hasMerged); /* try and merge clusters untill no cluster gets merged anymore */

		for(auto cluster : pos) {
			clusters.push_back(cluster.center());
		}

		pos.clear();

		return clusters;
	}
private:
	struct Cluster {
		std::vector<Position> objs;

		Cluster(Position p) {
			objs.push_back(p);
		}

		u64 merge(Cluster other) {
			for(auto p : other.objs) {
				objs.push_back(p);
			}

			return 0;
		}

		Position center() {
			Position pos;
			pos.pos = v3();
			pos.amplitude = 0;
			pos.frequency = 0;

			for(auto p : objs) {
				pos.pos += p.pos * p.amplitude;
				pos.amplitude += p.amplitude;
				pos.frequency += p.frequency;
			}

			pos.pos /= pos.amplitude;

			pos.amplitude /= objs.size();
			pos.frequency /= objs.size();

			return pos;
		}
	};

	// ToDo(robin): better function with direction more important than radius
	f64 dissimilarity(const Cluster & a, const Cluster & b) {
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

    // ToDo(robin): better way?
    u64 shouldMerge(const Cluster & a, const Cluster & b) {
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

	f64 maxClusterSize;

	std::deque<Cluster> pos;
	std::vector<Position> clusters;
};

#endif
