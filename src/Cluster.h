#ifndef _CLUSTER_H
#define _CLUSTER_H

#include <vector>

#include "Position.h"
#include "util/types.h"

class Cluster {
public:
    std::vector<Position> objs;

    Cluster(Position p);

    u64 merge(Cluster other);

    Position center() const;
};
#endif
