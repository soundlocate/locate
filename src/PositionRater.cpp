#include "PositionRater.h"

PositionRater::PositionRater(std::vector<Microfone> m) : mics(m) {}

// ToDo(robin): actually use the best position (-> use correct vs. located phase)
v3 PositionRater::bestPosition(std::vector<v3> & positions, FFTPacket & p) {
	return positions[0];
}
