#include "ChessViewModel.h"

ChessViewModel::ChessViewModel() {
  requestSocket_ = new QTcpSocket(this);
  responseServer_ = new QTcpServer(this);

  // STARTING VIEWMODEL'S RESPONSE SERVER
  if (!responseServer_->listen(QHostAddress::Any, responsePort_)) {
    qDebug() << "Response server could not start";
  } else {
    qDebug() << "Response server is listening on port " << responsePort_;
  }

  // CONNECTING TO CHESSSERVER'S REQUEST SERVER
  requestSocket_->connectToHost(hostIP_, requestPort_);
  if (requestSocket_->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << hostIP_
             << " and port: " << requestPort_;
    emit connected(true);
  } else {
    qDebug() << "Failed to connect to host on IP: " << hostIP_
             << " and port: " << requestPort_;
    emit connected(false);
  }

  // WAITING FOR CHESSSERVER'S TO CONNECT TO VIEWMODEL'S RESPONSE SERVER
  if (responseServer_->waitForNewConnection(1000)) {
    responseSocket_ = responseServer_->nextPendingConnection();
    qDebug() << "Successful connection to response server from ip:"
             << responseSocket_->peerAddress().toString()
             << " and port: " << responsePort_;
  } else {
    qDebug() << "No1 connected to response server on port: " << responsePort_;
  }
}

ChessViewModel::~ChessViewModel() {
  if (requestSocket_->isOpen()) {
    requestSocket_->close();
    qDebug() << "socket closed";
  }
}

QJsonObject ChessViewModel::sendRequest(QJsonObject request) {
  if (!requestSocket_->isWritable()) {
    qWarning() << "Socket is not writeable!";
    return {};
  }

  QJsonDocument doc(request);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  requestSocket_->write(data);
  requestSocket_->waitForBytesWritten(1000);

  if (request["Function"].toString() == QString("getField")) {
    if (responseSocket_->waitForReadyRead(1000)) {
      auto data = responseSocket_->readAll();

      return QJsonDocument::fromJson(data).object();
    }
  } else if (request["Function"].toString() == QString("possibleSteps")) {
    if (responseSocket_->waitForReadyRead(1000)) {
      auto data = responseSocket_->readAll();

      return QJsonDocument::fromJson(data).object();
    }
  }

  return QJsonObject();
}

QList<QPair<int, int>> ChessViewModel::possibleSteps(int x, int y,
                                                     bool includeDefendedPieces,
                                                     bool attack,
                                                     bool newTable) {
  QJsonObject request = {{"Function", "possibleSteps"},
                         {"Parameters", QJsonObject{{"x", x},
                                                    {"y", y},
                                                    {"includeDefendedPieces",
                                                     includeDefendedPieces},
                                                    {"attack", attack},
                                                    {"newTable", newTable}}}};

  auto responses = sendRequest(request);
  QList<QPair<int, int>> result;
  for (auto response : responses) {
    qDebug() << "POSSIBLESTEPS RESPONSE:\n" << response;
    result.append(QPair<int, int>(response.toObject()["First"].toInt(),
                                  response.toObject()["Second"].toInt()));
  }

  return result;
}

void ChessViewModel::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  QJsonObject request = {{"Function", "stepPiece"},
                         {"Parameters", QJsonObject{{"from_x", from_x},
                                                    {"from_y", from_y},
                                                    {"to_x", to_x},
                                                    {"to_y", to_y}}}};
  sendRequest(request);
}
void ChessViewModel::newGame() {
  QJsonObject request({{"Function", "newGame"}});
  sendRequest(request);
}

ChessField ChessViewModel::getField(int x, int y) {
  QJsonObject request({{"Function", "getField"},
                       {"Parameters", QJsonObject{{"x", x}, {"y", y}}}});

  auto response = sendRequest(request);
  qDebug() << "getField RESPONSE:\n" << response;

  ChessField f;
  if (response.contains("Field")) {
    auto fieldJSON = response["Field"].toObject();
    f._fieldColor = static_cast<FieldColor>(fieldJSON["FieldColor"].toInt());
    f._pieceColor = static_cast<PieceColor>(fieldJSON["PieceColor"].toInt());
    f._pieceType = static_cast<PieceTypes>(fieldJSON["PieceTypes"].toInt());
    f.highlighted = fieldJSON["Highlighted"].toBool();
    f.enPassant = fieldJSON["EnPassant"].toBool();
    f.hasMoved = fieldJSON["HasMoved"].toBool();
    f.isCastlingField = fieldJSON["IsCastlingField"].toBool();
  }
  return f;
}
void ChessViewModel::setHighlighted(int x, int y, bool highlight) {
  QJsonObject request(
      {{"Function", "setHighlighted"},
       {"Parameters",
        QJsonObject{{"x", x}, {"y", y}, {"highlight", highlight}}}});

  sendRequest(request);
}
void ChessViewModel::switchToQueen(int x, int y, PieceTypes switchTo) {
  QJsonObject request(
      {{"Function", "switchToQueen"},
       {"Parameters", QJsonObject{{"x", x},
                                  {"y", y},
                                  {"switchTo", static_cast<int>(switchTo)}}}});

  sendRequest(request);
}
