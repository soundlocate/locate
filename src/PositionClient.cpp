#include "PositionClient.h"

PositionClient::PositionClient(const char * ip, unsigned short port) {
	assert(std::strlen(ip) > 0);
	assert(port != 0);

	server = new sf::TcpSocket;

	// Connect to the server
	while(server->connect(sf::IpAddress(ip), port) != sf::Socket::Done) {
		//NOOP, lets just wait until the server comes up
	}
}

int PositionClient::sendPositions(std::vector<v3> positions) {
	u32	size = positions.size() * 2;

	server->send(&size, sizeof(u32));
	server->send(positions.data(), sizeof(v3) * size);
	server->send(positions.data(), sizeof(v3) * size);

	return 0;
}
