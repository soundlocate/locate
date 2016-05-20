#ifndef _LOCATOR3D_H
#define _LOCATOR3D_H

#include <array>
#include <cassert>
#include <cassert>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

#include "FFTPacket.h"
#include "Microfone.h"
#include "PositionRater.h"
#include "algorithms/Algorithm.h"
#include "util/constant.h"
#include "util/types.h"

class Locator3D {
public:
    Locator3D(std::vector<Microfone> microfones, std::vector<Algorithm *> a);

    v3 locate(FFTPacket::Sinus * packet);

private:
    u64 numMics;

    std::vector<Microfone>   mics;
    std::vector<Algorithm *> algorithms;
    PositionRater            rater;

    v3  center;
    f64 maxFreq;

    f64 f64Max = std::numeric_limits<f64>::max();
};

#endif
