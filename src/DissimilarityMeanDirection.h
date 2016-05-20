#ifndef _DISSIMILARITYMEANDIRECTION_H
#define _DISSIMILARITYMEANDIRECTION_H

#include <limits>
#include <vector>

#include "Dissimilarity.h"
#include "Microfone.h"
#include "util/types.h"

class DissimilarityMeanDirection : public Dissimilarity {
public:
    DissimilarityMeanDirection(std::vector<Microfone> mics, f64 maxClusterSize);

    // Note(robin): uses mean minimum angle
    f64 dissimilarity(const Cluster & a, const Cluster & b) override;

    u64 shouldMerge(const f64 & dissimilarity, const Cluster & a,
                    const Cluster & b) const override;

    Position center(const Cluster & a) const override;

private:
    v3              micCenter;
    f64             maxClusterSize;
    std::vector<v3> posABuffer;
    std::vector<v3> posBBuffer;

    f64 angleBetween(const v3 & a, const v3 & b) const;
};

#endif
