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
#include "ThreadedLocator.h"

#include <QtCore/QCoreApplication>

//ToDo(robin): implement log file
int main(int argc, char ** argv) {
	CommandLineOptions options(argc, argv);

	// logger
	Logger * log = nullptr;
	if(options.log())
		log = new Logger(options.logfilename());

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

	std::vector<v3> positionBuffer;

	// u64 i = 0;

	ThreadedLocator<12> locator(options);

	while(true) {
		positionBuffer.clear();

		for(auto pos : locator.getPositions()) {
			positionBuffer.push_back(pos.pos);

			if(wclient != nullptr)
				wclient->add(pos);

			if(log)
				log->log(pos.frequency, pos.pos);
		}

		posClient.sendPositions(positionBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
