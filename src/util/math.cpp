#include "math.h"

f64 math::deltaPhaseToDistance(f64 a, f64 b, f64 freq) {
	f64 diff = b - a;


	if(diff < -math::PI) {
		diff += 2 * math::PI;
	} else if(diff > math::PI) {
		diff -= 2 * math::PI;
	}

	diff *= sound::speedInAir;
	diff /= 2 * math::PI * freq;

	return diff;
}
