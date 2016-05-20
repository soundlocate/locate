#ifndef _FFTPACKET_H
#define _FFTPACKET_H

#include <cassert>
#include <cstring>

#include <iostream>

#include "util/types.h"

class FFTPacket {
public:
    struct Sinus {
        double freq;
        double phase;
        double amplitude;

        Sinus() : freq(0), phase(0), amplitude(0) {}

        Sinus(char * rawData) {
            assert(rawData != nullptr);

            memcpy(&freq, &rawData[0], sizeof(double));
            memcpy(&phase, &rawData[sizeof(double)], sizeof(double));
            memcpy(&amplitude, &rawData[2 * sizeof(double)], sizeof(double));
        }

        bool operator==(const Sinus & rhs) const {
            return (freq == rhs.freq) && (phase == rhs.phase) &&
                   (amplitude == rhs.amplitude);
        }

        bool operator!=(const Sinus & rhs) const {
            return (freq != rhs.freq) || (phase != rhs.phase) ||
                   (amplitude != rhs.amplitude);
        }
    };

    Sinus * sines;

    FFTPacket();

    FFTPacket(char * rawData, unsigned int count);

    bool operator==(const FFTPacket & rhs) const;

    bool operator!=(const FFTPacket & rhs) const;

    static f64 meanAmplitude(Sinus * s, u64 numMics);

    unsigned int getSineCount();

private:
    unsigned int sineCount = 0;
};

#endif
