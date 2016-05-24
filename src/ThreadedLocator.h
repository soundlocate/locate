#ifndef _THREADEDLOCATOR_H
#define _THREADEDLOCATOR_H

#include <mutex>
#include <thread>
#include <vector>

#include "CommandLineOptions.h"
#include "FFTPacket.h"
#include "FFTStream.h"
#include "Locator3D.h"
#include "Microfone.h"
#include "Position.h"
#include "PostProcessor.h"
#include "algorithms/AllCases.h"
#include "algorithms/PhaseOnly.h"
#include "util/types.h"

template <int numThreads>
class ThreadedLocator {
public:
    ThreadedLocator(CommandLineOptions options) {
        // init the mics
        double * micTmp = options.mics();

        for(u64 i = 0; i < options.micCount(); i++) {
            mics.push_back(
                Microfone(micTmp[3 * i], micTmp[3 * i + 1], micTmp[3 * i + 2]));
        }

        std::cout << "Microfones: " << std::endl << "[" << std::endl;

        for(u64 i = 0; i < options.micCount(); i++) {
            std::cout << mics[i].pos << "," << std::endl;
        }

        std::cout << "]" << std::endl;

        postProcessor = new PostProcessor(
            mics, options.clusterSize(), locate::maxDist,
            options.dissimilarityFunction(), options.meanWindow(),
            options.maxKeep(), options.keepTime());

        // Note(robin): uses ringbuffer and drops old packets
        stream = new FFTStream(options.fftIp().c_str(), options.fftPort());

        for(u64 i = 0; i < numThreads; i++) {
            locators.push_back(new std::thread([&]() {
                // locate algorithms
                std::vector<Algorithm *> algorithms;
                for(auto algorithm : options.algos()) {
                    switch(algorithm) {
                    case 0: {
                        algorithms.push_back((Algorithm *)new PhaseOnly(
                            options.micCount(), options.accuracy()));
                        break;
                    }
                    case 3: {
                        algorithms.push_back((Algorithm *)new AllCases(
                            mics, options.micCount(), options.accuracy()));
                        break;
                    }
                    default: {
                        std::cerr << "unhandled algorithm " << algorithm
                                  << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                    }
                }

                algorithms.push_back((Algorithm *)new PhaseOnly(
                    options.micCount(), options.accuracy()));

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
    std::vector<Microfone>     mics;
    std::vector<std::thread *> locators;

    PostProcessor * postProcessor;
    FFTStream *     stream;
    std::mutex      clusterMutex;
};

#endif
