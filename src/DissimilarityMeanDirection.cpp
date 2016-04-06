#include "DissimilarityMeanDirection.h"

DissimilarityMeanDirection::DissimilarityMeanDirection(std::vector<Microfone> mics, f64 maxClusterSize) :
	maxClusterSize(maxClusterSize) {
	// calculate center of mics
	v3 center;

	for(auto mic : mics) {
		center += mic.pos;
	}

	center /= mics.size();

	this->micCenter = center;
}

// Note(robin): uses mean minimum angle
f64 DissimilarityMeanDirection::dissimilarity(const Cluster & a, const Cluster & b) {
	f64 ret = 0;
	f64 tmp = 0;
	f64 min;
	v3 tmpPos;

	// calculate vector from center of mics
	posABuffer.clear();
	posBBuffer.clear();
	for(auto pos : a.objs) {
		posABuffer.push_back(pos.pos - micCenter);
	}
	for(auto pos : b.objs) {
		posBBuffer.push_back(pos.pos - micCenter);
	}

	for(auto posa : posABuffer) {
		min = 7; // > 2 * PI

		for(auto posb : posBBuffer) {
			tmp = angleBetween(posa, posb);
			if(tmp < min) min = tmp;
		}

		if(min < 7) ret += min; // check if any angle was calculated
	}

	ret /= a.objs.size() + b.objs.size();

	return ret;
}

u64 DissimilarityMeanDirection::shouldMerge(const f64 & dissimilarity, const Cluster & a, const Cluster & b) const {
	return dissimilarity < maxClusterSize;
}

Position DissimilarityMeanDirection::center(const Cluster & a) const {
	Position p;
	f64 anglex = 0;
	f64 anglez = 0;
	v3 xaxis(1, 0, 0);
	v3 zaxis(0, 0, 1);
	v3 tmp;

	p.amplitude = 0;
	p.frequency = 0;
	p.pos = v3();

	// cartesian to spherical (radius = 1)
	for(auto pos : a.objs) {
		tmp = pos.pos - micCenter;
		anglex += std::atan2(tmp.y, tmp.x) * pos.amplitude;
		anglez += std::acos(tmp.z / tmp.norm()) * pos.amplitude;

		p.amplitude += pos.amplitude;
		p.frequency += pos.frequency;
	}

	anglex /= p.amplitude;
	anglez /= p.amplitude;
	p.frequency /= a.objs.size();

	// spherical to cartesian (radius = 1)
	p.pos.x = sin(anglez) * cos(anglex);
	p.pos.y = sin(anglez) * sin(anglex);
	p.pos.z = cos(anglez);

	// ToDo(robin): better solution??
	p.pos += micCenter;

	return p;
}

f64 DissimilarityMeanDirection::angleBetween(const v3 & a, const v3 & b) const {
	return std::acos((a.x * b.x + a.y * b.y + a.z * b.z) / (a.norm() * b.norm()));
}
