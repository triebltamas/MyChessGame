#include "LocalChessWidget.h"
#include "backend/ChessModel.h"
#include "ui_LocalChessWidget.h"
#include "utilities/HoverEventFilter.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

LocalChessWidget::LocalChessWidget()
    : ui(new Ui::LocalChessWidget),
      model_(std::shared_ptr<ChessModel>(new ChessModel)) {
  ui->setupUi(this);
  // initUI
  ui->currentPlayerIconHolder->setIconSize(QSize(25, 25));
  ui->currentPlayerIconHolder->setIcon(QIcon(":/Application/KnightWhite"));
  ui->currentPlayerIconHolder->installEventFilter(new HoverEventFilter);
  ui->currentPlayerIconHolder->setFocusPolicy(Qt::NoFocus);
  chessTable = new ChessTableWidget(model_, this);
  ui->verticalLayout->addWidget(chessTable);
  ui->fenFrame->setVisible(enableFENImport_);
  connect(ui->importFenButton, &QPushButton::clicked, this, [this]() {
      model_->importFEN(ui->fenEdit->text());
  });

  connect(model_.get(), &ChessModel::gameOver, this,
          &LocalChessWidget::onGameOver, Qt::QueuedConnection);
  connect(model_.get(), &ChessModel::currentPlayerChanged, this,
          &LocalChessWidget::onCurrentPlayerChanged, Qt::DirectConnection);
  chessTable->newGame();
}

LocalChessWidget::~LocalChessWidget() { delete ui; }

void LocalChessWidget::onGameOver(int Player, int /*newElo*/,
                                  bool /*disconnected*/) {
  if (Player == 0) {
    QMessageBox::information(this, tr("Game over"), QString("Draw"));
  } else {
    QString msg = (Player == 1) ? QString("White won!") : QString("Black won!");
    QMessageBox::information(this, tr("Game over"), msg);
  }
  chessTable->newGame();
}

void LocalChessWidget::onCurrentPlayerChanged(int player) {
  if (player == 1)
    ui->currentPlayerIconHolder->setIcon(QIcon(":/Application/KnightWhite"));
  else
    ui->currentPlayerIconHolder->setIcon(QIcon(":/Application/KnightBlack"));
}
