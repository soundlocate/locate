#include "FFTPacket.h"

FFTPacket::FFTPacket() : sineCount(0) {}

// Note(robin): not really thread safe -> does not copy rawData
FFTPacket::FFTPacket(char * rawData, unsigned int count) : sineCount(count) {
    assert(count != 0);
    assert(rawData != nullptr);

    sines = (Sinus *)rawData;
}

bool FFTPacket::operator==(const FFTPacket & rhs) const {
    bool result = true;

    std::cout << "fftpacket operator==" << std::endl;

    if(sineCount == rhs.sineCount) {
        for(unsigned int i = 0; i < sineCount; i++) {
            result = result && (sines[i] == rhs.sines[i]);
        }

        return result;
    } else {
        return false;
    }
}

bool FFTPacket::operator!=(const FFTPacket & rhs) const {
    bool result = false;

    std::cout << "fftpacket operator!=" << std::endl;

    if(sineCount == rhs.sineCount) {
        for(unsigned int i = 0; i < sineCount; i++) {
            result = result || (sines[i] != rhs.sines[i]);
        }

        return result;
    } else {
        return true;
    }
}

f64 FFTPacket::meanAmplitude(Sinus * sines, u64 numMics) {
    double sum = 0;

    for(unsigned int i = 0; i < numMics; i++) { sum += sines[i].amplitude; }

    return sum / numMics;
}

unsigned int FFTPacket::getSineCount() { return sineCount; }
