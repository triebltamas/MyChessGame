#include "ChessViewModel.h"

ChessViewModel::ChessViewModel() {
  socket_ = new QTcpSocket(this);

  socket_->connectToHost(hostIP_, port_);
  if (socket_->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << hostIP_
             << " and port: " << port_;
    emit connected(true);
  } else {
    qDebug() << "Failed to connect to host on IP: " << hostIP_
             << " and port: " << port_;
    emit connected(false);
  }
}

void ChessViewModel::sendRequest(QString request) {
  if (!socket_->isWritable()) {
    qWarning() << "Socket is not writeable!";
    return;
  }

  QByteArray data;
  data.append(request);
  socket_->write(data);
  socket_->waitForBytesWritten(1000);
}

QList<QPair<int, int>> ChessViewModel::possibleSteps(int x, int y,
                                                     bool includeDefendedPieces,
                                                     bool attack,
                                                     bool newTable) {
  return {};
}

void ChessViewModel::stepPiece(int from_x, int from_y, int to_x, int to_y) {}
void ChessViewModel::newGame() {}
ChessField ChessViewModel::getField(int x, int y) {
  ChessField f;
  return f;
}
void ChessViewModel::setHighlighted(int x, int y, bool highlight) {}
void ChessViewModel::switchToQueen(int x, int y, PieceTypes switchTo) {}
