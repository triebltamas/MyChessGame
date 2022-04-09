#ifndef GAMESESSION_H
#define GAMESESSION_H
#include "UserSession.h"

#include <QTcpSocket>
struct GameSession {
  QString sessionID = "";
  bool sessionStarted = false;
  UserSession player1;
  UserSession player2;
};

#endif // GAMESESSION_H
