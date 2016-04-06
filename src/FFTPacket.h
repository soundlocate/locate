#ifndef _FFTPACKET_H
#define _FFTPACKET_H

#include <cassert>
#include <cstring>

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
			return (freq == rhs.freq) && (phase == rhs.phase) && (amplitude == rhs.amplitude);
		}

		bool operator!=(const Sinus & rhs) const {
			return (freq != rhs.freq) || (phase != rhs.phase) || (amplitude != rhs.amplitude);
		}
	};

    Sinus * sines;

	FFTPacket();

	FFTPacket(char * rawData, unsigned int count);

    FFTPacket(const FFTPacket& other);

    FFTPacket& operator=(const FFTPacket& other);

	bool operator==(const FFTPacket & rhs) const;

	bool operator!=(const FFTPacket & rhs) const;

	double meanAmplitude();

	unsigned int getSineCount();

	~FFTPacket();

private:
	unsigned int sineCount = 0;
};

#endif
