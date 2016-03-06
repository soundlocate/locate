#include <iostream>
#include <unordered_set>
#include <vector>

#include "util/types.h"
#include "util/constant.h"

#include "FFTStream.h"
#include "Locator3D.h"
#include "Microfone.h"
#include "PositionClient.h"
#include "WebsocketPositionClient.h"

#define MICCOUNT 4

int main(int argc, char ** argv) {
	double distBetween = 0.3253;

	std::array<Microfone, MICCOUNT> mics = {
//		Microfone(0.0, 0.0, 0.0),
		Microfone(0.0, distBetween , 0.0),
		Microfone(distBetween,0.0,0.0),
//		Microfone(distBetween, distBetween, 0.0),
		Microfone(0.0, 0.0, distBetween),
//		Microfone(distBetween, 0.0, distBetween),
//		Microfone(0.0, distBetween, distBetween),
		Microfone(distBetween, distBetween, distBetween),
	};

	std::cout << "Microfones: " << std::endl << "[" << std::endl;

	for(int i = 0; i < 8; i++) {
		std::cout << mics[i].pos << "," << std::endl;
	}

	std::cout << "]" << std::endl;

	Locator3D<MICCOUNT> locator(mics);
	FFTStream stream(argv[1], std::atoi(argv[2]));
	PositionClient posClient(argv[3], std::atoi(argv[4]));
	WebsocketPositionClient wclient(std::atoi(argv[5]));

	v3 pos;

	std::unordered_set<double> freqs;
	std::vector<v3> positionBuffer;

	int i = 0;

	for(auto packet : stream) {
		if(packet.sines[0].freq < 100 || packet.sines[0].freq > 600)
			continue;

		packet.sines[0] = packet.sines[1];
		packet.sines[1] = packet.sines[2];
		packet.sines[2] = packet.sines[4];
		packet.sines[3] = packet.sines[7];

		pos = locator.locate(packet);
		positionBuffer.push_back(pos);

		if(i % 5 == 0)
			wclient.send(v4(packet.sines[0].freq, pos.x, pos.y, pos.z));

		// locate the shit and if it already exists send it out
		if(!freqs.insert(packet.sines[0].freq).second) {
			i++;

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
