#include "OnlineChessWidget.h"
#include "ui_OnlineChessWidget.h"
#include "utilities/HoverEventFilter.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

OnlineChessWidget::OnlineChessWidget(
    std::shared_ptr<ChessAPIService> chessAPIService, IChessModel *model)
    : ui(new Ui::OnlineChessWidget), chessAPIService_(chessAPIService),
      model_(model) {
  ui->setupUi(this);
  ui->chatFrame->setVisible(false);
  connect(
      model_, &IChessModel::PieceStepped, this,
      [this](int from_x, int from_y, int to_x, int to_y) {
        if (model_->getCurrentPlayer() !=
            chessAPIService_->getGameSessionID().second)
          return;

        chessAPIService_->stepPiece(from_x, from_y, to_x, to_y);
      },
      Qt::DirectConnection);

  connect(
      model_, &IChessModel::PieceSwitched, this,
      [this](PieceTypes piece) {
        if (model_->getCurrentPlayer() !=
            chessAPIService_->getGameSessionID().second)
          return;

        chessAPIService_->onPieceSwitched(piece);
      },
      Qt::DirectConnection);

  connect(model_, &IChessModel::gameOver, this, [this](int Player) {
    if (model_->getCurrentPlayer() ==
        chessAPIService_->getGameSessionID().second)
      return;

    QJsonObject request = {{"Function", "gameOver"},
                           {"Parameters", QJsonObject{{"Player", Player}}}};
    chessAPIService_->sendRequest(request);
  });

  chessTable_ = new ChessTableWidget(model_, this);

  ui->verticalLayout->addWidget(chessTable_);
  connect(chessAPIService_.get(), &ChessAPIService::opponentStepped, this,
          [this](int from_x, int from_y, int to_x, int to_y) {
            model_->stepPiece(from_x, from_y, to_x, to_y);
          });

  connect(chessAPIService_.get(), &ChessAPIService::opponentSwitched, this,
          [this](int x, int y, PieceTypes piece) {
            if (model_->getCurrentPlayer() !=
                chessAPIService_->getGameSessionID().second)
              return;

            model_->switchToQueen(x, y, piece);
          });
  connect(chessAPIService_.get(), &ChessAPIService::startGame, this,
          &OnlineChessWidget::onStartGame);
  connect(chessAPIService_.get(), &ChessAPIService::gameOver, this,
          &OnlineChessWidget::onGameOver);
  connect(chessTable_, &ChessTableWidget::currentPlayerChanged, this,
          &OnlineChessWidget::updateStatusLabel);
  chessAPIService_->startQueueing();
}

OnlineChessWidget::~OnlineChessWidget() { delete ui; }

void OnlineChessWidget::onGameOver(int Player, int newElo, bool disconnected) {
  if (disconnected) {
    QMessageBox::information(
        this, tr("Game over"),
        QString("Opponent surrendered!\nNew elo: %1").arg(newElo));
  } else if (Player == 0) {
    QMessageBox::information(this, tr("Game over"),
                             QString("Draw!\nNew elo: %1").arg(newElo));
  } else {
    QString msg = (Player == fixedPlayerNumber_)
                      ? QString("You won!\nNew elo: %1").arg(newElo)
                      : QString("You lost!\nNew elo: %1").arg(newElo);
    QMessageBox::information(this, tr("Game over"), msg);
  }
  // todo go to main menu
}

void OnlineChessWidget::onStartGame(int fixedPlayerNumber) {
  fixedPlayerNumber_ = fixedPlayerNumber;

  chessTable_->newGame();
  ui->player1Label->setText(QString("%1: %2 elo")
                                .arg(chessAPIService_->getUsername())
                                .arg(chessAPIService_->getElo()));
  ui->player2Label->setText(QString("%1: %2 elo")
                                .arg(chessAPIService_->getOpponentsUsername())
                                .arg(chessAPIService_->getOpponentsElo()));
  ui->chatFrame->setVisible(true);
  updateStatusLabel(1);

  qDebug() << "STARTING GAME";
}

void OnlineChessWidget::updateStatusLabel(int player) {
  if (player == fixedPlayerNumber_) {
    ui->statusLabel->setText("You may step...");
    chessTable_->updateTableEnabled(true);
  } else {
    ui->statusLabel->setText("Waiting for opponet to step...");
    chessTable_->updateTableEnabled(false);
  }
}
