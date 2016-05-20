#ifndef _DISSIMILARITY_H
#define _DISSIMILARITY_H

#include "Cluster.h"
#include "Position.h"

class Dissimilarity {
public:
    virtual f64 dissimilarity(const Cluster & a, const Cluster & b) = 0;
    virtual u64 shouldMerge(const f64 & dissimilarity, const Cluster & a,
                            const Cluster & b) const = 0;
    virtual Position center(const Cluster & a) const = 0;
};

#endif
