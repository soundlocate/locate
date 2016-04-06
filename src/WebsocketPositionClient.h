#ifndef _WEBSOCKETPOSITIONCLIENT_H
#define _WEBSOCKETPOSITIONCLIENT_H

#include <iostream>
#include <set>
#include <mutex>
#include <thread>
#include <cstring>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "util/types.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class WebsocketPositionClient {
public:
	WebsocketPositionClient(unsigned short port);

	int send(v4 pos);

	~WebsocketPositionClient();
private:
	typedef websocketpp::server<websocketpp::config::asio> websocket_server;

	websocket_server server;
	std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
	std::mutex mutex;

	void on_open(websocketpp::connection_hdl hdl);

	void on_close(websocketpp::connection_hdl hdl);
};

#endif
