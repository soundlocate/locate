#ifndef _WEBSOCKETPOSITIONCLIENT_H
#define _WEBSOCKETPOSITIONCLIENT_H

#include <iostream>
#include <set>
#include <mutex>
#include <thread>
#include <cstring>

#include "Position.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketPositionClient : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketPositionClient(quint16 port, bool debug = false, QObject *parent = Q_NULLPTR);
    ~WebsocketPositionClient();

	int send(Position pos);

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    bool m_debug;
};


#endif
