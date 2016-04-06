#include "FFTPacket.h"

FFTPacket::FFTPacket() : sineCount(0) {}

FFTPacket::FFTPacket(char * rawData, unsigned int count) : sineCount(count) {
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

FFTPacket::FFTPacket(const FFTPacket& other) :
	sines(new Sinus[other.sineCount]), sineCount(other.sineCount) {
	std::memcpy(sines, other.sines, sineCount * sizeof(Sinus));
}


FFTPacket& FFTPacket::operator=(const FFTPacket& other) {
	if (this != &other) {
		if(sineCount > 0)
			delete[] sines;

		sines = new Sinus[other.sineCount];
		std::memcpy(sines, other.sines, other.sineCount * sizeof(Sinus));
		sineCount = other.sineCount;
	}

	return *this;
}

bool FFTPacket::operator==(const FFTPacket & rhs) const {
	bool result = true;

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

	if(sineCount == rhs.sineCount) {
		for(unsigned int i = 0; i < sineCount; i++) {
			result = result || (sines[i] != rhs.sines[i]);
		}

		return result;
	} else {
		return true;
	}
}

double FFTPacket::meanAmplitude() {
	double sum = 0;

	for (unsigned int i = 0; i < sineCount; i++) {
		sum += sines[i].amplitude;
	}

	return sum / sineCount;
}

unsigned int FFTPacket::getSineCount() {
	return sineCount;
}

FFTPacket::~FFTPacket() {
	if(sineCount > 0) {
		delete[] sines;
	}
}
