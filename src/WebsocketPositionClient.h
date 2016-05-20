#ifndef _WEBSOCKETPOSITIONCLIENT_H
#define _WEBSOCKETPOSITIONCLIENT_H

#include <cstring>
#include <deque>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

#include "Position.h"

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore>
#include <QtWebSockets/qwebsocket.h>
#include <QtWebSockets/qwebsocketserver.h>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketPositionClient : public QObject {
    Q_OBJECT
public:
    explicit WebsocketPositionClient(quint16 port, bool debug = false,
                                     QObject * parent = Q_NULLPTR);
    ~WebsocketPositionClient();

    int send(Position pos);

    int add(Position pos);
Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void socketDisconnected();
    void sendPackets() {
        std::lock_guard<std::mutex> lock(positionMutex);

        while(positions.size() > 0) {
            send(positions[0]);
            positions.pop_front();
        }
    }

private:
    QWebSocketServer *  m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    bool                m_debug;

    std::deque<Position> positions;

    std::mutex positionMutex;
};

#endif
