#ifndef _FFTSTREAM_H
#define _FFTSTREAM_H

#include "FFTPacket.h"
#include <cstring>
#include <SFML/Network.hpp>

class FFTStream : public std::iterator<std::input_iterator_tag, FFTPacket> {
public:
	FFTStream(const char * ip, unsigned short port) {
		assert(std::strlen(ip) > 0);
		assert(port != 0);

		server = new sf::TcpSocket;
		server->setBlocking(true);

		// Connect to the server
		while(server->connect(sf::IpAddress(ip), port) != sf::Socket::Done) {
			//NOOP, lets just wait until the server comes up
		}

		status = connected;

		// Connected, now lets get the mic count
		std::size_t received_size;
	    server->receive(&numMic, sizeof(unsigned int), received_size);
		assert(received_size == sizeof(unsigned int));

		++(*this);
	}

	unsigned int numMics() {
		return numMic;
	};

	FFTStream& operator++() {
		std::size_t received_size;
		std::size_t to_receive_size = numMic * 3 * sizeof(double);

		// allocate buffer
		char * rawData = new char[to_receive_size];

		// receive one packet
		if(server->receive(rawData, to_receive_size, received_size) == sf::Socket::Disconnected) {
			status = eos; // the server closed
		}
		assert(received_size == to_receive_size);

		// convert raw data to packet
		currentPacket = FFTPacket(rawData, numMic);

		// deallocate buffer
		delete[] rawData;

		return *this;
	}

	bool operator==(const FFTStream& rhs) {
		if(rhs.status == status)
			return currentPacket == rhs.currentPacket;
		else
			return rhs.status == status;
	}

	bool operator!=(const FFTStream& rhs) {
		if(rhs.status == status)
			return currentPacket != rhs.currentPacket;
		else
			return rhs.status != status;
	}

    FFTPacket& operator*() {
		return currentPacket;
	}

    FFTStream begin() {
		return *this;
	}

    FFTStream end() {
		return FFTStream(eos);
	}

	enum FFTSteamStatus {
		disconnected,
		connected,
		eos
	};

private:
	FFTStream(FFTSteamStatus newStatus) : status(newStatus), currentPacket() {}

	sf::TcpSocket * server;
	unsigned int numMic;
	FFTPacket currentPacket;
	FFTSteamStatus status = disconnected;
};

#endif
