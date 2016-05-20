#include "WebsocketPositionClient.h"

QT_USE_NAMESPACE

WebsocketPositionClient::WebsocketPositionClient(quint16 port, bool debug,
                                                 QObject * parent)
    : QObject(parent),
      m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Echo Server"),
                                              QWebSocketServer::NonSecureMode,
                                              this)),
      m_clients(), m_debug(debug) {
    if(m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        if(m_debug) qDebug() << "Echoserver listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this,
                &WebsocketPositionClient::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this,
                &WebsocketPositionClient::closed);
    }
}

WebsocketPositionClient::~WebsocketPositionClient() {
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void WebsocketPositionClient::onNewConnection() {
    QWebSocket * pSocket = m_pWebSocketServer->nextPendingConnection();

    qDebug() << "client connected" << pSocket;

    connect(pSocket, &QWebSocket::disconnected, this,
            &WebsocketPositionClient::socketDisconnected);

    m_clients << pSocket;
}

void WebsocketPositionClient::socketDisconnected() {
    QWebSocket * pClient = qobject_cast<QWebSocket *>(sender());
    if(m_debug) qDebug() << "socketDisconnected:" << pClient;
    if(pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

int WebsocketPositionClient::add(Position pos) {
    std::lock_guard<std::mutex> lock(positionMutex);

    positions.push_back(pos);

	return 0;
}

int WebsocketPositionClient::send(Position pos) {
    double  data_conv[5];
    int64_t orig = 0, res = 0;

    // position
    for(int i = 0; i < 3; i++) {
        memcpy(&orig, &pos.pos.comp[i], sizeof(double));
        res = __builtin_bswap64(orig);
        memcpy(&data_conv[i], &res, sizeof(double));
    }

    // amplitude
    memcpy(&orig, &pos.amplitude, sizeof(double));
    res = __builtin_bswap64(orig);
    memcpy(&data_conv[3], &res, sizeof(double));

    // frequency
    memcpy(&orig, &pos.frequency, sizeof(double));
    res = __builtin_bswap64(orig);
    memcpy(&data_conv[4], &res, sizeof(double));

    for(auto client : m_clients) {
        client->sendBinaryMessage(
            QByteArray::fromRawData((char *)&data_conv[0], sizeof(double) * 5));
        // server.send(hdl, data_conv, 5 * sizeof(double),
        // websocketpp::frame::opcode::binary);
    }

    return 0;
}
