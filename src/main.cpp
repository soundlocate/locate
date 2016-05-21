#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <thread>
#include <unordered_set>
#include <vector>

#include <cstdio>
#include <ctime>

#include "util/constant.h"
#include "util/types.h"

#include "CommandLineOptions.h"
#include "Logger.h"
#include "PositionClient.h"
#include "ThreadedLocator.h"
#include "WebsocketPositionClient.h"

#include <QtCore/QCoreApplication>

// ToDo(robin): is it worth to rebuild fftstream + threaded locator eventbased
// -> in FFTStream call the ThreadedLocator callback
// callback: deque (own non resizing version...) of available locators, use the
// first available, if non available drop the packet...

// ToDo(robin): implement log file
int main(int argc, char ** argv) {
    CommandLineOptions options(argc, argv);

    // logger
    Logger * log          = nullptr;
    if(options.log()) log = new Logger(options.logfilename());

    PositionClient posClient(options.guiIp().c_str(), options.guiPort());

    WebsocketPositionClient * wclient = nullptr;

    auto handle = std::thread([&]() {
        QCoreApplication a(argc, argv);

        wclient = new WebsocketPositionClient(options.websocketPort(), true);

        QTimer * timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), wclient,
                         SLOT(sendPackets()));
        timer->start(100);

        a.exec();
    });

    std::vector<v3>    positionBuffer;
    ThreadedLocator<1> locator(options);

    while(true) {
        positionBuffer.clear();

        for(auto pos : locator.getPositions()) {
            positionBuffer.push_back(pos.pos);

            if(wclient != nullptr) wclient->add(pos);

            if(log) log->log(pos.frequency, pos.pos);
        }

        posClient.sendPositions(positionBuffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
