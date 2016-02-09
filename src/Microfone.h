#ifndef _MICROFONE_H
#define _MICROFONE_H

#include "util/types.h"

class Microfone {
public:
	Microfone(): pos() {};
	Microfone(v3 position): pos(position) {};
	Microfone(double x, double y, double z) : pos(x, y, z) {};

	v3 pos;
};

#endif
