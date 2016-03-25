#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>

#include <cstdio>
#include <ctime>

#include "util/types.h"
#include "util/constant.h"

#include "FFTStream.h"
#include "Locator3D.h"
#include "Microfone.h"
#include "PositionClient.h"
#include "WebsocketPositionClient.h"
#include "CommandLineOptions.h"

#define MICCOUNT 8

int main(int argc, char ** argv) {
	double distBetween = 0.42;

	CommandLineOptions options(argc, argv);

	std::array<Microfone, MICCOUNT> mics = {
		Microfone(0.0, 0.0, 0.0),
		Microfone(0.0, distBetween , 0.0),
		Microfone(distBetween,0.0,0.0),
		Microfone(distBetween, distBetween, 0.0),
		Microfone(0.0, 0.0, distBetween),
		Microfone(distBetween, 0.0, distBetween),
		Microfone(0.0, distBetween, distBetween),
		Microfone(distBetween, distBetween, distBetween),
	};

	char filename[256];
	sprintf(filename, "%ld", time(0));

	FILE * outFile = fopen(filename, "r");

	std::cout << "Microfones: " << std::endl << "[" << std::endl;

	for(int i = 0; i < 8; i++) {
		std::cout << mics[i].pos << "," << std::endl;
	}

	std::cout << "]" << std::endl;

	Locator3D<MICCOUNT> locator(mics);
	FFTStream stream(options.fftIp().c_str(), options.fftPort());
	PositionClient posClient(options.guiIp().c_str(), options.guiPort());
	WebsocketPositionClient wclient(options.websocketPort());

	v3 pos;

	std::unordered_set<double> freqs;
	std::vector<v3> positionBuffer;

	int i = 0;

	for(auto packet : stream) {
		if(packet.sines[0].freq < 50 || packet.sines[0].freq > 600)
			continue;

		pos = locator.locate(packet);
		positionBuffer.push_back(pos);

		fprintf(outFile, "%ld, %lf, %lf, %lf, %lf\n", time(0), packet.sines[0].freq, pos.x, pos.y, pos.z);
		fflush(outFile);

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
