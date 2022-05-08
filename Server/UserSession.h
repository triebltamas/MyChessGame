#ifndef USERSESSION_H
#define USERSESSION_H

#include <QTcpSocket>
#include <QTimer>
struct UserSession {
  QString sessionID = "";
  QString username = "";
  bool inGame = false;
  QTcpSocket *requestSocket = nullptr;
  QTcpSocket *responseSocket = nullptr;
  QTcpSocket *heartbeatSocket = nullptr;
  QTimer *heartbeatTimer = nullptr;
};

#endif // USERSESSION_H
