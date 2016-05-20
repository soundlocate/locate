#include "FFTStream.h"

using namespace std::chrono_literals;

FFTStream::FFTStream(const char * ip, unsigned short port) {
    assert(std::strlen(ip) > 0);
    assert(port != 0);

    server = new sf::TcpSocket;
    server->setBlocking(false);

    // Connect to the server
    while(server->connect(sf::IpAddress(ip), port) != sf::Socket::Done) {
        // NOOP, lets just wait until the server comes up
    }

    status = connected;

    // Connected, now lets get the mic count
    std::size_t received_size;
    server->receive(&numMic, sizeof(unsigned int), received_size);
    assert(received_size == sizeof(unsigned int));

    to_receive_size = numMic * 3 * sizeof(double);

    // ToDo(robin): config option of size
    buffer = new DoubleRingBuffer<FFTPacket::Sinus>(numMic, 65536);

    // start receiving
    handle = new std::thread([&]() {
        std::size_t        receivedSize = 0;
        sf::Socket::Status s;
        std::size_t        toReceive = to_receive_size;
        char *             buf       = (char *)buffer->pushPtr();

        // receive packets
        while(true) {
            // as long as some data is available
            while(server->receive(&buf[to_receive_size - toReceive], toReceive,
                                  receivedSize) != sf::Socket::NotReady) {
                toReceive -= receivedSize;

                // receive a whole packet
                while(toReceive > 0) {
                    s = server->receive(&buf[to_receive_size - toReceive],
                                        toReceive, receivedSize);

                    if(s != sf::Socket::Done) {
                        std::cout << "fftserver disconnected" << std::endl;
                        status = eos; // the server closed

                        exit(0);
                    } else {
                        toReceive -= receivedSize;
                    }
                }

                buffer->pushDone();

                toReceive = to_receive_size;

                buf = (char *)buffer->pushPtr();
            }
        }

        std::this_thread::sleep_for(10ms);
    });

    handle->detach();

    ++(*this);
}

FFTStream::FFTStream(FFTSteamStatus newStatus)
    : currentPacket(), status(newStatus) {}

unsigned int & FFTStream::numMics() { return numMic; };

FFTStream & FFTStream::operator++() {
    // convert raw data into packet
    currentPacket = buffer->popPtr();

    return *this;
}

// always return false -> we want an infinite loop
bool FFTStream::operator==(const FFTStream & rhs) { return false; }

// the end will never be reached
bool FFTStream::operator!=(const FFTStream & rhs) { return true; }

FFTPacket::Sinus * FFTStream::operator*() { return currentPacket; }

FFTStream & FFTStream::begin() { return *this; }

FFTStream & FFTStream::end() { return *this; }
