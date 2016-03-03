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
	WebsocketPositionClient(unsigned short port) {
		assert(std::strlen(ip) > 0);
		assert(port != 0);

		server.clear_access_channels(websocketpp::log::alevel::all);
		server.set_open_handler(bind(&WebsocketPositionClient::on_open, this, ::_1));
		server.set_close_handler(bind(&WebsocketPositionClient::on_close, this, ::_1));

		server.init_asio();
		server.listen(port);
		server.start_accept();

		std::cout << "localhost:" << port << std::endl;

		std::thread t([&]() {
				server.run();
			});
		t.detach();
	}

	int send(v4 pos) {
		std::lock_guard<std::mutex> lock(mutex);

		double * data_conv[4];
		int64_t orig = 0, res = 0;

		for (int i = 0; i < 4; i++) {
			memcpy(&orig, &pos.comp[i], sizeof(double));
			res = __builtin_bswap64(orig);
			memcpy(&data_conv[i], &res, sizeof(double));
		}

		for(auto hdl : connections) {
			server.send(hdl, data_conv, 4 * sizeof(double), websocketpp::frame::opcode::binary);
		}
	}

private:
	typedef websocketpp::server<websocketpp::config::asio> websocket_server;
	websocket_server server;
	std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
	std::mutex mutex;

	void on_open(websocketpp::connection_hdl hdl) {
		std::lock_guard<std::mutex> lock(mutex);
		connections.insert(hdl);
	}

	void on_close(websocketpp::connection_hdl hdl) {
		std::lock_guard<std::mutex> lock(mutex);
		connections.erase(hdl);
	}
};

#endif
