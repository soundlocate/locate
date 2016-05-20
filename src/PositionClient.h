#ifndef _POSITIONCLIENT_H
#define _POSITIONCLIENT_H

#include <cassert>

#include <cstring>

#include <SFML/Network.hpp>

#include "util/types.h"

class PositionClient {
public:
    PositionClient(const char * ip, unsigned short port);

    int sendPositions(std::vector<v3> positions);

private:
    sf::TcpSocket * server;
};

#endif
