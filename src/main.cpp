#include <iostream>
#include <unordered_set>
#include <vector>

#include "util/types.h"
#include "util/constant.h"

#include "FFTStream.h"
#include "Locator3D.h"
#include "Microfone.h"
#include "PositionClient.h"

int main(int argc, char ** argv) {
	double distBetween = 0.28;

	std::array<Microfone, 4> mics = {
			Microfone(0.0, 0.0, 0.0),
			Microfone(0.0, distBetween , 0.0),
			Microfone(sin((60.0 / 180.0) * math::PI) * distBetween , distBetween  / 2, 0.0),
			Microfone(tan((30.0 / 180.0) * math::PI) * (distBetween / 2.0), distBetween  / 2.0, (1.0 / 3.0) * sqrt(6.0) * distBetween)
	};

	std::cout << "Microfones: " << std::endl << "[" << std::endl;

	for(int i = 0; i < 4; i++) {
		std::cout << mics[i].pos << "," << std::endl;
	}

	std::cout << "]" << std::endl;

	Locator3D<4> locator(mics);
	FFTStream stream(argv[1], std::atoi(argv[2]));
	PositionClient posClient(argv[3], std::atoi(argv[4]));
	v3 pos;

	std::unordered_set<double> freqs;
	std::vector<v3> positionBuffer;

	for(auto packet : stream) {
		if(packet.sines[0].freq < 300 || packet.sines[0].freq > 1200)
			continue;

		pos = locator.locate(packet);
		positionBuffer.push_back(pos);

		// locate the shit and if it already exists send it out
		if(!freqs.insert(packet.sines[0].freq).second) {
			// delete last position, it will be duplicate
			positionBuffer.erase(positionBuffer.end() - 1);

			posClient.sendPositions(positionBuffer);
		    freqs.clear();
			positionBuffer.clear();

			// add last position as first new position
			positionBuffer.push_back(pos);
		}
	}
}
