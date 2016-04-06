#ifndef _MATHS_H
#define _MATHS_H

#include "types.h"
#include "constant.h"

namespace math {
	f64 deltaPhaseToDistance(f64 a, f64 b, f64 freq) {
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
}

#endif
