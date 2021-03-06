#ifndef _ALGORITHM_H
#define _ALGORITHM_H

#include <vector>

#include "../FFTPacket.h"
#include "../Microfone.h"
#include "../util/types.h"

class Algorithm {
public:
    virtual v3 locate(std::vector<Microfone> mics, FFTPacket::Sinus * p,
                      v3 center) = 0;
};

#endif
