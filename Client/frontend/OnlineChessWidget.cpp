#include "OnlineChessWidget.h"
#include "ui_OnlineChessWidget.h"
#include "utilities/HoverEventFilter.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

OnlineChessWidget::OnlineChessWidget(
    std::shared_ptr<ChessAPIService> chessAPIService)
    : ui(new Ui::OnlineChessWidget), chessAPIService_(chessAPIService) {
  ui->setupUi(this);
  chessTable_ = new ChessTableWidget(chessAPIService_, this);
  ui->verticalLayout->addWidget(chessTable_);
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
