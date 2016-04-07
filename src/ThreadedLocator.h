#ifndef _THREADEDLOCATOR_H
#define _THREADEDLOCATOR_H

#include <thread>
#include <vector>
#include <mutex>

#include "Microfone.h"
#include "Position.h"
#include "FFTPacket.h"
#include "CommandLineOptions.h"
#include "util/types.h"

template<int numThreads>
class ThreadedLocator {
public:
	ThreadedLocator(CommandLineOptions options) {
		// init the mics
		double * micTmp = options.mics();

		for(u64 i = 0; i < options.micCount(); i++) {
			mics.push_back(Microfone(micTmp[3 * i], micTmp[3 * i + 1], micTmp[3 * i + 2]));
		}

		std::cout << "Microfones: " << std::endl << "[" << std::endl;

		for(u64 i = 0; i < options.micCount(); i++) {
			std::cout << mics[i].pos << "," << std::endl;
		}

		std::cout << "]" << std::endl;


		postProcessor = new PostProcessor(mics, options.clusterSize(), locate::maxDist, options.dissimilarityFunction(),  options.meanWindow(), options.maxKeep(), options.keepTime());

		// Note(robin): uses ringbuffer and drops old packets
		stream = new FFTStream(options.fftIp().c_str(), options.fftPort());

		for(u64 i = 0; i < numThreads; i++) {
			locators.push_back(new std::thread([&](){
						// locate algorithms
						std::vector<Algorithm *> algorithms;
						algorithms.push_back((Algorithm *) new PhaseOnly(options.micCount(), options.accuracy()));

						Locator3D locator(mics, algorithms);

						v3 pos;

						for(const auto & packet : *stream) {
							pos = locator.locate(packet);

							{
								std::lock_guard<std::mutex> lock(clusterMutex);

								postProcessor->add(packet, pos);
							}
						}
					}));
		}
	}

	std::vector<Position> & getPositions() {
		return postProcessor->getPositions();
	}
private:
	std::vector<Microfone> mics;
	std::vector<std::thread *> locators;

	PostProcessor * postProcessor;
	FFTStream *	stream;
	std::mutex clusterMutex;
};

#endif
