#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>

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
#include "Logger.h"
#include "algorithms/Algorithm.h"
#include "algorithms/PhaseOnly.h"
#include "PostProcessor.h"
#include "RingBuffer.h"

#include <Stopwatch.h>

//ToDo(robin): implement log file
int main(int argc, char ** argv) {
	CommandLineOptions options(argc, argv);

	Stopwatch::getInstance().setCustomSignature(32436);

	std::vector<Microfone> mics;
	double * micTmp = options.mics();

	for(u64 i = 0; i < options.micCount(); i++) {
		mics.push_back(Microfone(micTmp[3 * i], micTmp[3 * i + 1], micTmp[3 * i + 2]));
	}

	Logger * log = nullptr;

	if(options.log())
		log = new Logger(options.logfilename());

  	std::cout << "Microfones: " << std::endl << "[" << std::endl;
    
	for(u64 i = 0; i < options.micCount(); i++) {
		std::cout << mics[i].pos << "," << std::endl;
	}

	std::cout << "]" << std::endl;

	std::vector<Algorithm *> algorithms;
	algorithms.push_back((Algorithm *) new PhaseOnly(options.micCount(), options.accuracy()));

	Locator3D locator( mics, algorithms);



	// 0.2m maximum cluster size, 3 meanWindow, 10 maxKeep, 0.5 seconds value keep
	// ToDo(robin):	finish value keep time support
	PostProcessor postProcessor(mics, options.clusterSize(), locate::maxDist, options.dissimilarityFunction(),  options.meanWindow(), options.maxKeep(), options.keepTime());

	// ToDo(robin): use ringbuffer and drop old packets
	FFTStream stream(options.fftIp().c_str(), options.fftPort());

	v3 pos;

	PositionClient posClient(options.guiIp().c_str(), options.guiPort());
	WebsocketPositionClient wclient(options.websocketPort());

	std::unordered_set<double> freqs;
	std::vector<v3> positionBuffer;

	// u64 i = 0;

	TICK("locate_total");
	for(auto packet : stream) {
		TOCK("locate_total");
		TICK("locate_total");

		TICK("locate_locate");
		pos = locator.locate(packet);
		TOCK("locate_locate");

		TICK("locate_other_bullshit");

		postProcessor.add(packet, pos);

		positionBuffer.clear();

		for(auto pos : postProcessor.getPositions()) {
			positionBuffer.push_back(pos.pos);

      wclient.send(v4(pos.frequency, pos.pos.x, pos.pos.y, pos.pos.z));

			if(log)
				log->log(pos.frequency, pos.pos);
		}

		posClient.sendPositions(positionBuffer);

		// check for sensible values

		// if(pos.norm() < locate::maxDist) {
		// 	positionBuffer.push_back(pos);

		// 	// write position to log
		// 	if(log)
		// 		log->log(packet.sines[0].freq, pos);

		// 	// send position to the websocket clients
		// 	wclient.send(v4(packet.sines[0].freq, pos.x, pos.y, pos.z));

		// 	// locate the shit and if it already exists send it out
		// 	if(!freqs.insert(packet.sines[0].freq).second) {
		// 		i++;

		// 		// delete last position, it will be duplicate
		// 		positionBuffer.erase(positionBuffer.end() - 1);

		// 		posClient.sendPositions(positionBuffer);
		// 		freqs.clear();
		// 		positionBuffer.clear();

		// 		// add last position as first new position
		// 		positionBuffer.push_back(pos);
		// 	}
		// }

    
		TOCK("locate_other_bullshit");

		Stopwatch::getInstance().sendAll();
	}
}
