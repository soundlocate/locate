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
		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "clustering " << positions.size() << " objs" << std::endl;


	    clusters.clear();
		pos.clear();

		for(u64 i = 0; i < positions.size(); i++) {
			pos.push_back(Cluster(positions.nextPosition()));
		}

	    u64 other = 0;
		f64 dsim = std::numeric_limits<f64>::max();
		f64 dsimTmp;

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "pos.size(): " << pos.size() << std::endl;

		while(pos.size() > 1) {
			for(u64 i = 0; i < pos.size(); i++) {
				for(u64 j = 0; j < pos.size(); j++) {
					// do not merge the clusters with itself
					if(i == j)
						continue;

					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "i: " << i << " j: " << j << std::endl;


					dsimTmp = dissimilarity(pos[i], pos[j]);

					if(dsimTmp < dsim) {
						dsim = dsimTmp;
						other = j;
					}
				}

				if(shouldMerge(pos[i], pos[other])) {
					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "cluster should merge: " << i << "with: " << other << std::endl;

					pos[i].merge(pos[other]);

					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "pos.size(): " << pos.size() << std::endl;

					pos.erase(pos.begin() + other);

					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "pos.size() after erase: " << pos.size() << std::endl;
				} else {
					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "cluster should not merge: " << i << "with: " << other << std::endl;

					clusters.push_back(pos[i].center());
					clusters.push_back(pos[other].center());

					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "pos.size(): " << pos.size() << std::endl;

					pos.erase(pos.begin() + i);
					pos.erase(pos.begin() + other);

					std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
							  << "pos.size() after erase: " << pos.size() << std::endl;
				}

				dsim = std::numeric_limits<f64>::max();
				other = 0;
			}
		}

		if(pos.size() == 1)
			clusters.push_back(pos[0].center());

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

			pos.pos /= objs.size();
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

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << "dissimilarity of: " << a.objs[0].pos
				  << "and: " << b.objs[0].pos;

		for(auto posa : a.objs) {
			for(auto posb : b.objs) {
				tmp = (posa.pos - posb.pos).norm();
				ret += tmp * tmp;
			}
		}

		ret =  sqrt(ret / (a.objs.size() * b.objs.size()));

		std::cout << "[" << __PRETTY_FUNCTION__ << "]: "
				  << ": " << ret << std::endl;

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
