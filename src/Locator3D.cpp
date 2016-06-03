#include "Locator3D.h"

Locator3D::Locator3D(std::vector<Microfone>   microfones,
                     std::vector<Algorithm *> a)
    : numMics(microfones.size()), mics(microfones), algorithms(a), rater(mics) {

    assert(mics.size() == numMics);

    std::cout << "numMics: " << numMics << std::endl;

    // calculate maximum frequency
    f64 maxDist = std::numeric_limits<double>::min();
    f64 curDist = 0;

    for(u64 i = 0; i < numMics; i++) {
        for(u64 j = i + 1; j < numMics; j++) {
            curDist = (microfones[i].pos - microfones[j].pos).norm();
            maxDist = curDist > maxDist ? curDist : maxDist;
        }
    }

    // f = c / lambda
    maxFreq = sound::speedInAir / maxDist;

    std::cout << "maxFreq: " << maxFreq << std::endl;

    // calculate geometrical center of all microfones
    for(u64 i = 0; i < numMics; i++) { center += microfones[i].pos; }

    center /= numMics;

    std::cout << "center: " << center << std::endl;
}

// locate a packet
v3 Locator3D::locate(FFTPacket::Sinus * packet) {
    std::vector<v3> positions;

	// when using allcases this should not get used
	// ToDo(robin): move this to individual algorithms
    // if(packet[0].freq > maxFreq) return v3(f64Max, f64Max, f64Max);
	if(packet[0].freq > maxFreq) return v3(f64Max, f64Max, f64Max);

    for(auto a : algorithms) {
        positions.push_back(a->locate(mics, packet, center));
    }

    // std::cout << packet.sines[0].freq << ":" << rater.bestPosition(positions,
    // packet) << std::endl;

    return rater.bestPosition(positions, packet);
};
