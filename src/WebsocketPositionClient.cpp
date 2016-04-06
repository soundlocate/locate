#include "WebsocketPositionClient.h"

WebsocketPositionClient::WebsocketPositionClient(unsigned short port) {
	assert(port != 0);

	server.clear_access_channels(websocketpp::log::alevel::all);
	server.set_open_handler(bind(&WebsocketPositionClient::on_open, this, ::_1));
	server.set_close_handler(bind(&WebsocketPositionClient::on_close, this, ::_1));

	server.init_asio();
	server.listen(port);
	server.start_accept();

	std::cout << "binding websocket to " << port << std::endl;

	std::thread t([&]() {
			server.run();
		});
	t.detach();
}

int WebsocketPositionClient::send(v4 pos) {
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

	return 0;
}

WebsocketPositionClient::~WebsocketPositionClient() {
	server.stop();
}

void WebsocketPositionClient::on_open(websocketpp::connection_hdl hdl) {
	std::lock_guard<std::mutex> lock(mutex);
	connections.insert(hdl);
}

void WebsocketPositionClient::on_close(websocketpp::connection_hdl hdl) {
	std::lock_guard<std::mutex> lock(mutex);
	connections.erase(hdl);
}
