#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <thread>

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

#include <QtCore/QCoreApplication>

//ToDo(robin): implement log file
int main(int argc, char ** argv) {
	CommandLineOptions options(argc, argv);

	std::vector<Microfone> mics;
	double * micTmp = options.mics();

	for(u64 i = 0; i < options.micCount(); i++) {
		mics.push_back(Microfone(micTmp[3 * i], micTmp[3 * i + 1], micTmp[3 * i + 2]));
	}

	// logger
	Logger * log = nullptr;
	if(options.log())
		log = new Logger(options.logfilename());

  	std::cout << "Microfones: " << std::endl << "[" << std::endl;
    
	for(u64 i = 0; i < options.micCount(); i++) {
		std::cout << mics[i].pos << "," << std::endl;
	}

	std::cout << "]" << std::endl;

	// locate algorithms
	std::vector<Algorithm *> algorithms;
	algorithms.push_back((Algorithm *) new PhaseOnly(options.micCount(), options.accuracy()));

	Locator3D locator( mics, algorithms);

	// postprocessor
	PostProcessor postProcessor(mics, options.clusterSize(), locate::maxDist, options.dissimilarityFunction(),  options.meanWindow(), options.maxKeep(), options.keepTime());

	// ToDo(robin): use ringbuffer and drop old packets
	FFTStream stream(options.fftIp().c_str(), options.fftPort());

	v3 pos;

	PositionClient posClient(options.guiIp().c_str(), options.guiPort());

	WebsocketPositionClient * wclient = nullptr;

	auto handle = std::thread([&]() {
			QCoreApplication a(argc, argv);

			wclient = new WebsocketPositionClient(options.websocketPort(), true);

			QTimer *timer = new QTimer();
			QObject::connect(timer, SIGNAL(timeout()), wclient, SLOT(sendPackets()));
			timer->start(100);

			a.exec();
		});

	std::unordered_set<double> freqs;
	std::vector<v3> positionBuffer;

	// u64 i = 0;

	for(const auto & packet : stream) {
		pos = locator.locate(packet);

		postProcessor.add(packet, pos);

		positionBuffer.clear();

		for(auto pos : postProcessor.getPositions()) {
			positionBuffer.push_back(pos.pos);

			if(wclient != nullptr)
				wclient->add(pos);

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
	}
}
