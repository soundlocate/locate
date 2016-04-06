#include "Cluster.h"

Cluster::Cluster(Position p) {
	objs.push_back(p);
}

u64 Cluster::merge(Cluster other) {
	for(auto p : other.objs) {
		objs.push_back(p);
	}

	return 0;
}

Position Cluster::center() const {
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
