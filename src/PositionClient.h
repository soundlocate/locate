#ifndef _POSITIONCLIENT_H
#define _POSITIONCLIENT_H

#include <SFML/Network.hpp>
#include "util/types.h"

class PositionClient {
public:
	PositionClient(const char * ip, unsigned short port) {
		assert(std::strlen(ip) > 0);
		assert(port != 0);

		server = new sf::TcpSocket;

		// Connect to the server
		while(server->connect(sf::IpAddress(ip), port) != sf::Socket::Done) {
			//NOOP, lets just wait until the server comes up
		}
	}

	int sendPositions(std::vector<v3> positions) {
		u32	size = positions.size();

		server->send(&size, sizeof(int));
		server->send(positions.data(), sizeof(v3) * size);

		return 0;
	}

private:
	sf::TcpSocket * server;
};

#endif
