#ifndef _FFTSTREAM_H
#define _FFTSTREAM_H

#include <cstring>

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <thread>

#include <SFML/Network.hpp>

#include "DoubleRingBuffer.h"
#include "FFTPacket.h"

class FFTStream : public std::iterator<std::input_iterator_tag, FFTPacket> {
public:
    FFTStream(const char * ip, unsigned short port);

    unsigned int & numMics();

    FFTStream & operator++();

    bool operator==(const FFTStream & rhs);

    bool operator!=(const FFTStream & rhs);

    FFTPacket::Sinus * operator*();

    FFTStream & begin();

    FFTStream & end();

    enum FFTSteamStatus { disconnected, connected, eos };

private:
    FFTStream(FFTSteamStatus newStatus);
    DoubleRingBuffer<FFTPacket::Sinus> * buffer;
    std::thread *                        handle;

    std::size_t to_receive_size;

    sf::TcpSocket *    server;
    unsigned int       numMic;
    FFTPacket::Sinus * currentPacket;
    FFTSteamStatus     status = disconnected;
};

#endif
