#ifndef USERSESSION_H
#define USERSESSION_H

#include <QTcpSocket>
struct UserSession {
  QString sessionID = "";
  QString username = "";
  bool inGame = false;
  QTcpSocket *requestSocket = nullptr;
  QTcpSocket *responseSocket = nullptr;
};

#endif // USERSESSION_H