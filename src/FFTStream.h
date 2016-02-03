#ifndef _FFTSTREAM_H
#define _FFTSTREAM_H

#include "FFTPacket.h"
#include <cstring>
#include <SFML/Network.hpp>

class FFTStream : public std::iterator<std::input_iterator_tag, FFTPacket>{
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

		// Connected, now lets get the mic count
		std::size_t received_size;
	    server->receive(&numMics, sizeof(unsigned int), received_size);
		assert(received_size == sizeof(unsigned int));
	}

	unsigned int numMics() {
		return numMics;
	};

	FFTStream& operator++() {
		std::size_t received_size;
		std::size_t to_receive_size = count * 3 * sizeof(double);

		char * rawData = new char[to_receive_size];

		server->receive(rawData, to_receive_size, received_size);
		assert(received_size == to_receive_size);

		return *this;
	}

private:
	sf::TcpSocket * server;
	unsigned int numMics;
	FFTPacket currentPacket;
};

#endif
