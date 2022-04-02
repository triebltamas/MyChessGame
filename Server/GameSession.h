#ifndef GAMESESSION_H
#define GAMESESSION_H

#include <QTcpSocket>
struct GameSession {
    QString sessionID = "";
    bool sessionStarted = false;
    QTcpSocket* player1RequestSocket = nullptr;
    QTcpSocket* player1ResponseSocket = nullptr;
    QTcpSocket* player2RequestSocket = nullptr;
    QTcpSocket* player2ResponseSocket = nullptr;
};

#endif // GAMESESSION_H
