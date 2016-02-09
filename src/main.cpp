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
	double distBetween = 1;

	std::array<Microfone, 4> mics = {
			Microfone(0.0, 0.0, 0.0),
			Microfone(0.0, distBetween , 0.0),
			Microfone(sin((60.0 / 180.0) * math::PI) * distBetween , distBetween  / 2, 0.0),
//			Microfone(1, 0, 0.66 * (1.0 / 3.0) * sqrt(6.0) * distBetween)
			Microfone(tan((30.0 / 180.0) * math::PI) * (distBetween / 2.0), distBetween  / 2.0, (1.0 / 3.0) * sqrt(6.0) * distBetween)
	};

	Locator3D<4> locator(mics);
	FFTStream stream(argv[1], std::atoi(argv[2]));
	PositionClient posClient(argv[3], std::atoi(argv[4]));
	v3 pos;

	std::unordered_set<double> freqs;
	std::vector<v3> positionBuffer;

	for(auto packet : stream) {
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
