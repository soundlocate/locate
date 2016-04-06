#ifndef _POSITION_H
#define _POSITION_H

#include <functional>
#include "util/types.h"

class Position {
public:
	v3 pos;
	f64 amplitude;
	f64 frequency;
};

namespace std {
	template<>
	struct hash<Position> : public std::unary_function<const Position&, std::size_t> {
		inline std::size_t operator()(const Position& p) const {
			std::size_t seed = 0;
			hash_combine(seed, p.amplitude);
			hash_combine(seed, p.frequency);
			hash_combine(seed, p.pos);

			return seed;
		}
	};
}

#endif
