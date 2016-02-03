#ifndef _FFTPACKET_H
#define _FFTPACKET_H

#include <cassert>

class FFTPacket {
public:
	struct Sinus {
		double freq;
		double phase;
		double amplitude;

		Sinus() : freq(0), phase(0), amplitude(0) {}

		Sinus(unsigned char * rawData) :
			freq((double) *rawData),
			phase((double) *(rawData + sizeof(double))),
			amplitude((double) *(rawData + 2 *sizeof(double))) {
			assert(rawData != nullptr);
		}
	};

    Sinus * sines;

	FFTPacket(unsigned char * rawData, unsigned int count) {
		assert(count != 0);
		assert(rawData != nullptr);

		sines = new Sinus[count];

		for(int i = 0; i < count; i++) {
			sines[i] = Sinus(rawData); // new sinus from rawdata
			rawData += sizeof(Sinus);  // increment pointer to rawdata,
			                           // so that it points to the data of the next sinus
		}
	}

	~FFTPacket() {
		delete[] sines;
	}
};

#endif
