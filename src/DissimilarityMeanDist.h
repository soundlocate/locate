#ifndef _DISSIMILARITYMEANDIST_H
#define _DISSIMILARITYMEANDIST_H

#include <limits>

#include "Dissimilarity.h"
#include "Position.h"

class DissimilarityMeanDist : public Dissimilarity {
public:
	DissimilarityMeanDist(f64 maxClusterSize);

	f64 dissimilarity(const Cluster & a, const Cluster & b) override;

	u64 shouldMerge(const f64 & dissimilarity, const Cluster & a, const Cluster & b) const override;

	Position center(const Cluster & a) const override;
private:
	f64 maxClusterSize;
};

#endif
