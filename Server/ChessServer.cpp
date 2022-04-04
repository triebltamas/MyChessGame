#include "ChessServer.h"
#include <QThread>

ChessServer::ChessServer(QObject *parent) : QObject(parent) {
  server_ = new QTcpServer(this);
  randomGenerator = new QRandomGenerator(1234);

  connect(server_, &QTcpServer::newConnection, this,
          &ChessServer::onNewConnection);

  // port_ = 1337
  if (!server_->listen(QHostAddress::Any, requestPort_)) {
    qDebug() << "Server could not start";
  } else {
    qDebug() << "Server is listening on port " << requestPort_;
  }
}

ChessServer::~ChessServer() {
  //  for (auto session : gameSessions_) {
  //    onDisconnected(session.sessionID);
  //  }
  if (server_->isListening()) {
    qDebug() << "Closing server";
    server_->close();
    server_->deleteLater();
  }
  if (randomGenerator != nullptr)
    delete randomGenerator;
}

void ChessServer::onGameOver(QString sessionID, int sessionPlayer,
                             int winnerPlayer) {}
void ChessServer::onCheck(QString sessionID, int sessionPlayer) {}

void ChessServer::onDisconnected(QString key) {
  if (!userSessions_.contains(key))
    return;
  auto requestSocket = userSessions_[key].requestSocket;
  auto responseSocket = userSessions_[key].responseSocket;

  if (requestSocket != nullptr && requestSocket->isOpen()) {
    requestSocket->close();
    requestSocket->deleteLater();
  }
  if (responseSocket != nullptr && responseSocket->isOpen()) {
    responseSocket->close();
    responseSocket->deleteLater();
  }
}

void ChessServer::onNewConnection() {
  QTcpSocket *requestSocket = server_->nextPendingConnection();

  connect(requestSocket, &QTcpSocket::readyRead, this, [=]() {
    auto data = requestSocket->readAll();
    //    qDebug() << data;

    QString str = QString(data);
    str = str.replace("\n", "");
    QJsonDocument itemDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject request = itemDoc.object();
    QString sessionID = request["GameSessionID"].toString();
    int senderPlayer = request["SessionPlayer"].toInt();

    QString func = request["Function"].toString();
    QJsonObject parameters = request["Parameters"].toObject();

    if (func == "responsePort") {
      onResponseSockectAvailable(requestSocket->peerAddress(),
                                 parameters["Port"].toInt(), requestSocket);
      return;
    }
    if (func == "login") {
      loginUser(parameters["UserSessionID"].toString(),
                parameters["Username"].toString(),
                parameters["Password"].toString());
      return;
    }
    if (func == "signUp") {
      createUser(parameters["UserSessionID"].toString(),
                 parameters["Email"].toString(),
                 parameters["Username"].toString(),
                 parameters["Password"].toString());
      return;
    }
    if (func == "endGameSession") {
      endGameSession(parameters["UserSessionID"].toString());
      return;
    }

    QTcpSocket *socket = nullptr;
    if (senderPlayer == 1)
      socket = gameSessions_[sessionID].player2.responseSocket;
    else
      socket = gameSessions_[sessionID].player1.responseSocket;

    if (socket == nullptr)
      return;

    socket->write(data);
  });
}

void ChessServer::onResponseSockectAvailable(QHostAddress address,
                                             int responsePort,
                                             QTcpSocket *requestSocket) {
  QTcpSocket *responseSocket = new QTcpSocket(this);
  responseSocket->connectToHost(address, responsePort);
  if (responseSocket->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << address.toString()
             << " and port: " << responsePort;

    QString newSessionID;

    int counter = 0;
    while (counter < 1000) {
      newSessionID = QString::number(randomGenerator->generate());
      if (!userSessions_.contains(newSessionID))
        break;

      if (counter > 250) {
        qDebug() << "Generator did not seed";
        randomGenerator->seed();
      }
      if (counter > 500) {
        qDebug() << "GENERATOR DID NOT SEED";
        randomGenerator->seed(4324);
      }
      counter++;
    }

    UserSession session;
    session.sessionID = newSessionID;
    session.requestSocket = requestSocket;
    session.responseSocket = responseSocket;

    userSessions_[newSessionID] = session;

    QJsonDocument doc(QJsonObject{
        {"Function", "connected"},
        {"Parameters", QJsonObject{{"UserSessionID", newSessionID}}}});
    QByteArray data;
    data.append(QString::fromLatin1(doc.toJson()));
    responseSocket->write(data);
    responseSocket->waitForBytesWritten(1000);

    connect(responseSocket, &QTcpSocket::disconnected, this, [=]() {
      if (userSessions_[newSessionID].inGame)
        endGameSession(newSessionID);

      onDisconnected(newSessionID);
    });

    connect(requestSocket, &QTcpSocket::disconnected, this, [=]() {
      if (userSessions_[newSessionID].inGame)
        endGameSession(newSessionID);

      onDisconnected(newSessionID);
    });

  } else {
    qDebug() << "Failed to connect to host on IP: " << address.toString()
             << " and port: " << responsePort;
  }
}

void ChessServer::onStartQueueing(QString userSessionID) {
  auto requestSocket = userSessions_[userSessionID].requestSocket;
  auto responseSocket = userSessions_[userSessionID].responseSocket;
  bool hasConnected = false;
  for (auto session : gameSessions_) {
    if (!session.sessionStarted) {
      session.player2.requestSocket = requestSocket;
      session.player2.responseSocket = responseSocket;
      session.sessionStarted = true;
      hasConnected = true;

      // send start to both clients
      QJsonObject json1;
      json1.insert("Function", "startGame");
      json1.insert("Parameters",
                   QJsonObject{{"GameSessionID", session.sessionID},
                               {"SessionPlayerNumber", 1}});

      QJsonDocument doc1(json1);
      QByteArray data1;
      data1.append(QString::fromLatin1(doc1.toJson()));
      session.player1.responseSocket->write(data1);
      session.player1.responseSocket->waitForBytesWritten(1000);

      QJsonObject json2;
      json2.insert("Function", "startGame");
      json2.insert("Parameters",
                   QJsonObject{{"GameSessionID", session.sessionID},
                               {"SessionPlayerNumber", 2}});

      QJsonDocument doc2(json2);
      QByteArray data2;
      data2.append(QString::fromLatin1(doc2.toJson()));
      session.player2.responseSocket->write(data2);
      session.player2.responseSocket->waitForBytesWritten(1000);

      gameSessions_[session.sessionID] = session;
      break;
    }
  }
  if (!hasConnected) {
    QString newGameSessionID;

    int counter = 0;
    while (counter < 1000) {
      newGameSessionID = QString::number(randomGenerator->generate());
      if (!gameSessions_.contains(newGameSessionID))
        break;

      if (counter > 250) {
        qDebug() << "Generator did not seed";
        randomGenerator->seed();
      }
      if (counter > 500) {
        qDebug() << "GENERATOR DID NOT SEED";
        randomGenerator->seed(4324);
      }
      counter++;
    }
    GameSession newSession;
    newSession.sessionID = newGameSessionID;
    newSession.player1 = userSessions_[userSessionID];
    gameSessions_[newGameSessionID] = newSession;

    qDebug() << "newGameSessionID: " << newGameSessionID;
  }
}

void ChessServer::endGameSession(QString userSessionID) {
  QString gameSessionID = "";
  for (auto session : gameSessions_) {
    if (session.player1.sessionID == userSessionID) {
      QJsonObject json;
      json.insert("Function", "userDisconnected");

      QJsonDocument doc(json);
      QByteArray data;
      data.append(QString::fromLatin1(doc.toJson()));
      session.player2.responseSocket->write(data);
      session.player2.responseSocket->waitForBytesWritten(1000);

      gameSessionID = session.sessionID;

      break;

    } else if (session.player2.sessionID == userSessionID) {
      QJsonObject json;
      json.insert("Function", "userDisconnected");

      QJsonDocument doc(json);
      QByteArray data;
      data.append(QString::fromLatin1(doc.toJson()));
      session.player1.responseSocket->write(data);
      session.player1.responseSocket->waitForBytesWritten(1000);

      gameSessionID = session.sessionID;
      break;
    }
  }
  gameSessions_.remove(gameSessionID);
}

void ChessServer::loginUser(QString userSessionID, QString username,
                            QString password) {
  // itt kell a dbbol majd lekerdezni a a jelszavakat meg a felhasznaloneveket
  QJsonObject json;
  json.insert("Function", "loginSuccess");
  json.insert("Parameters", QJsonObject{{"Success", true}, {"Message", ""}});
  QJsonDocument doc(json);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  userSessions_[userSessionID].responseSocket->write(data);
  userSessions_[userSessionID].responseSocket->waitForBytesWritten(1000);
}

void ChessServer::createUser(QString userSessionID, QString email,
                             QString username, QString password) {
  // itt kell a dbvel megnezni h lehet e regisztralni
  QJsonObject json;
  json.insert("Function", "createSuccess");
  json.insert("Parameters", QJsonObject{{"Success", true}, {"Message", ""}});
  QJsonDocument doc(json);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  userSessions_[userSessionID].responseSocket->write(data);
  userSessions_[userSessionID].responseSocket->waitForBytesWritten(1000);
}
