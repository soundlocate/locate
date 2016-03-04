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

#define MICCOUNT 5

int main(int argc, char ** argv) {
	double distBetween = 0.28*2;

	double dx = 0, dy = 0, dz = 0;

	{
		double pos[4 * 3] = {
			0, 0, 0,
			0, 1, 0,
			sin(M_PI * 0.333333), 0.5, 0,
			0.5 * tan(M_PI * 0.161616), 0.5, 0.333333 * sqrt(6)
		};

		for(int i = 0; i < 4; i++) {
			dx += pos[3 * i] * distBetween;
			dy += pos[3 * i + 1] * distBetween;
			dz += pos[3 * i + 2] * distBetween;
		}

		dx /= 4;
		dy /= 4;
		dz /= 4;

		double tx = 0, ty = 0, tz = 0;

		for(int i = 0; i < 4; i++) {
			tx += pos[3 * i] * distBetween * 0.5;
			ty += pos[3 * i + 1] * distBetween * 0.5;
			tz += pos[3 * i + 2] * distBetween * 0.5;
		}

		tx /= 4;
		ty /= 4;
		tz /= 4;

		dx -= tx;
		dy -= ty;
		dz -= tz;
	}

	std::array<Microfone, MICCOUNT> mics = {
		Microfone(0.0, 0.0, 0.0),
		Microfone(0.0, distBetween , 0.0),
		Microfone(sin((60.0 / 180.0) * math::PI) * distBetween , distBetween  / 2, 0.0),
		Microfone(tan((30.0 / 180.0) * math::PI) * (distBetween / 2.0), distBetween  / 2.0, (1.0 / 3.0) * sqrt(6.0) * distBetween),
		Microfone(dx, dy, dz),
//		Microfone(dx, dy  + distBetween * 0.5, dz),
//		Microfone(dx + sin((60.0 / 180.0) * math::PI) * distBetween * 0.5, dy + distBetween  / 4, dz),
//		Microfone(dx + tan((30.0 / 180.0) * math::PI) * (distBetween / 4.0), dy + distBetween  / 4.0, dz - (1.0 / 3.0) * sqrt(6.0) * distBetween * 0.5),
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
//		if(packet.sines[0].freq < 300 || packet.sines[0].freq > 1200)
//			continue;

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
