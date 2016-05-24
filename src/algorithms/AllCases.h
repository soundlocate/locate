#ifndef _ALL_CASES_H
#define _ALL_CASES_H

#include <cassert>
#include <iostream>
#include <limits>
#include <unordered_map>

#include "/opt/intel/mkl/include/mkl_lapacke.h"

#include "../util/constant.h"
#include "../util/math.h"
#include "Algorithm.h"

class AllCases : public Algorithm {
public:
    AllCases(std::vector<Microfone> mics, u64 numMics, f64 accuracy);

    v3 locate(std::vector<Microfone> mics, FFTPacket::Sinus * p,
              v3 center) override;

    ~AllCases();

private:
    std::unordered_map<f64, v3> lastPositions;

    u64 numMics;
    i32 rank;

    f64 accuracy = 0.01;
	f64 maxFreq = 2000.0;
    f64 f64Max   = std::numeric_limits<f64>::max();

    f64 * target;
    f64 * singular;
    f64 * matrix;
    f64 * x;
    f64 * r;
};

#endif
