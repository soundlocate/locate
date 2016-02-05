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
			memcpy(&freq, &rawData[0], sizeof(double));
			memcpy(&phase, &rawData[sizeof(double)], sizeof(double));
			memcpy(&amplitude, &rawData[2 * sizeof(double)], sizeof(double));
			assert(rawData != nullptr);
		}

		bool operator==(const Sinus & rhs) const {
			return (freq == rhs.freq) && (phase == rhs.phase) && (amplitude == rhs.amplitude);
		}

		bool operator!=(const Sinus & rhs) const {
			return (freq != rhs.freq) || (phase != rhs.phase) || (amplitude != rhs.amplitude);
		}
	};

    Sinus * sines;

	FFTPacket() : sineCount(0) {}

	FFTPacket(char * rawData, unsigned int count) : sineCount(count) {
		assert(count != 0);
		assert(rawData != nullptr);

		sines = new Sinus[count];

		memcpy(sines, rawData, sizeof(Sinus) * count);

		//for(int i = 0; i < count; i++) {
		//	sines[i] = Sinus(rawData); // new sinus from rawdata
		//	rawData += sizeof(Sinus);  // increment pointer to rawdata,
			                           // so that it points to the data of the next sinus
		//}
	}

    FFTPacket(const FFTPacket& other) :
		sines(new Sinus[other.sineCount]), sineCount(other.sineCount) {
		std::memcpy(sines, other.sines, sineCount * sizeof(Sinus));
	}

    FFTPacket& operator=(const FFTPacket& other) {
		if (this != &other) {
			if(sineCount > 0)
				delete[] sines;

			sines = new Sinus[other.sineCount];
			std::memcpy(sines, other.sines, other.sineCount * sizeof(Sinus));
			sineCount = other.sineCount;
		}

		return *this;
	}

	bool operator==(const FFTPacket & rhs) const {
		bool result = true;

		if(sineCount == rhs.sineCount) {
			for(int i = 0; i < sineCount; i++) {
				result = result && (sines[i] == rhs.sines[i]);
			}

			return result;
		} else {
			return false;
		}
	}

	bool operator!=(const FFTPacket & rhs) const {
		bool result = false;

		if(sineCount == rhs.sineCount) {
			for(int i = 0; i < sineCount; i++) {
				result = result || (sines[i] != rhs.sines[i]);
			}

			return result;
		} else {
			return true;
		}
	}

	double meanAmplitude() {
		double sum = 0;

		for (int i = 0; i < sineCount; i++) {
			sum += sines[i].amplitude;
		}

		return sum / sineCount;
	}

	unsigned int getSineCount() {
		return sineCount;
	}

	~FFTPacket() {
		if(sineCount > 0) {
			delete[] sines;
		}
	}

private:
	unsigned int sineCount = 0;
};

#endif
