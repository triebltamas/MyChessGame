#include "OnlineChessWidget.h"
#include "HoverEventFilter.h"
#include "ui_OnlineChessWidget.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

OnlineChessWidget::OnlineChessWidget(ChessAPIService *chessAPIService)
    : ui(new Ui::OnlineChessWidget), chessAPIService_(chessAPIService) {
  ui->setupUi(this);
  connect(chessAPIService_, &ChessAPIService::startGame, this,
          &OnlineChessWidget::onStartGame);
  connect(chessAPIService_, &ChessAPIService::refreshTable, this,
          &OnlineChessWidget::onRefreshTable);
  connect(chessAPIService_, &ChessAPIService::gameOver, this,
          &OnlineChessWidget::onGameOver);
  connect(chessAPIService_, &ChessAPIService::pawnHasReachedEnemysBase, this,
          &OnlineChessWidget::onPawnHasReachedEnemysBase);
  connect(chessAPIService_, &ChessAPIService::check, this,
          &OnlineChessWidget::onCheck);
  chessAPIService->startQueueing();
}

OnlineChessWidget::~OnlineChessWidget() { delete ui; }

void OnlineChessWidget::newGame() {
  chessAPIService_->newGame();
  generateTable();
}

void OnlineChessWidget::generateTable() {
  int i = 0;
  for (int j = 0; j < 8 && i < 8; ++j && ++i) {
    if (tableView_[i * 8 + j] != nullptr)
      delete tableView_[i * 8 + j];
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      tableView_.insert(i * 8 + j, new QPushButton());
      tableView_[i * 8 + j]->setMinimumSize(QSize(100, 100));
      tableView_[i * 8 + j]->setSizeIncrement(QSize(1, 1));
      tableView_[i * 8 + j]->setIconSize(QSize(100, 100));
      tableView_[i * 8 + j]->installEventFilter(new HoverEventFilter);

      ui->gridLayout->addWidget(tableView_[i * 8 + j], i, j);
      updateCell(i, j, chessAPIService_->getField(i, j), true);

      connect(tableView_[i * 8 + j], &QPushButton::clicked, this,
              [this, i, j]() { onCellClicked(i, j); });
    }
  }
  ui->centralWidget->setStyleSheet(
      "QWidget {background-color: rgb(255, 206, 112);}");
}

void OnlineChessWidget::updateCell(int x, int y, ChessField field,
                                   bool initField) {
  if (initField) {
    switch (field._fieldColor) {
    case FieldColor::Black:
      tableView_[x * 8 + y]->setStyleSheet(
          "QPushButton { background-color: rgb(150, 82, 33); border: 0px;} ");
      break;
    case FieldColor::White:
      tableView_[x * 8 + y]->setStyleSheet(
          "QPushButton { background-color: rgb(255, 214, 173); border: 0px;} ");
      break;
    default:
      break;
    }
  }

  switch (field._pieceType) {
  case PieceTypes::King:
    tableView_[x * 8 + y]->setText("");
    if (field._pieceColor == PieceColor::White)
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/KingWhite"));
    else
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/KingBlack"));
    break;
  case PieceTypes::Queen:
    if (field._pieceColor == PieceColor::White)
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/QueenWhite"));
    else
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/QueenBlack"));
    break;
  case PieceTypes::Bishup:
    if (field._pieceColor == PieceColor::White)
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/BishupWhite"));
    else
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/BishupBlack"));
    break;
  case PieceTypes::Knight:
    if (field._pieceColor == PieceColor::White)
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/KnightWhite"));
    else
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/KnightBlack"));
    break;
  case PieceTypes::Rook:
    if (field._pieceColor == PieceColor::White)
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/RookWhite"));
    else
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/RookBlack"));
    break;
  case PieceTypes::Pawn:
    if (field._pieceColor == PieceColor::White)
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/PawnWhite"));
    else
      tableView_[x * 8 + y]->setIcon(QIcon(":/Application/PawnBlack"));
    break;
  case PieceTypes::VoidType:
    tableView_[x * 8 + y]->setIcon(QIcon());
    break;
  default:
    break;
  }
}

void OnlineChessWidget::onGameOver(int Player, int newElo) {
  if (Player == 0) {
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

void OnlineChessWidget::onCellClicked(int x, int y) {
  if (chessAPIService_->getField(x, y)._pieceColor == PieceColor::VoidColor &&
      !chessAPIService_->getField(x, y).highlighted)
    return;

  if (green_) {
    if (x == clickedCell_.first && y == clickedCell_.second) {
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          updateCell(i, j, chessAPIService_->getField(i, j), true);
          chessAPIService_->setHighlighted(i, j, false);
        }
      }

      green_ = false;
    } else {
      if (chessAPIService_->getField(x, y).highlighted) {
        chessAPIService_->stepPiece(clickedCell_.first, clickedCell_.second, x,
                                    y);

        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            updateCell(i, j, chessAPIService_->getField(i, j), true);
            chessAPIService_->setHighlighted(i, j, false);
          }
        }
        green_ = false;
        updateStatusLabel();
      } else {
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            updateCell(i, j, chessAPIService_->getField(i, j), true);
            chessAPIService_->setHighlighted(i, j, false);
          }
        }

        if (!chessAPIService_->isMyPiece(x, y))
          return;

        auto cells = chessAPIService_->possibleSteps(x, y, false, true, false);
        if (!cells.empty())
          cells.append(QPair<int, int>(x, y));

        for (auto cell : cells) {
          if (chessAPIService_->getField(cell.first, cell.second)._fieldColor ==
              FieldColor::White) {
            tableView_[cell.first * 8 + cell.second]->setStyleSheet(
                "QPushButton { background-color: rgb(0, 184, 44); border: "
                "0px;} ");
          } else {

            tableView_[cell.first * 8 + cell.second]->setStyleSheet(
                "QPushButton { background-color: rgb(0, 140, 33); border: "
                "0px;}");
          }

          chessAPIService_->setHighlighted(cell.first, cell.second, true);
        }

        green_ = true;

        clickedCell_.first = x;
        clickedCell_.second = y;
      }
    }

  } else {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        updateCell(i, j, chessAPIService_->getField(i, j), true);
      }
    }

    if (!chessAPIService_->isMyPiece(x, y))
      return;

    auto cells = chessAPIService_->possibleSteps(x, y, false, true, false);
    if (!cells.empty())
      cells.append(QPair<int, int>(x, y));

    for (auto cell : cells) {
      if (chessAPIService_->getField(cell.first, cell.second)._fieldColor ==
          FieldColor::White) {
        tableView_[cell.first * 8 + cell.second]->setStyleSheet(
            "QPushButton { background-color: rgb(0, 184, 44); border: "
            "0px;} ");
      } else {

        tableView_[cell.first * 8 + cell.second]->setStyleSheet(
            "QPushButton { background-color: rgb(0, 140, 33); border: "
            "0px;}");
      }

      chessAPIService_->setHighlighted(cell.first, cell.second, true);
    }

    green_ = true;

    clickedCell_.first = x;
    clickedCell_.second = y;
  }
}

void OnlineChessWidget::onPawnHasReachedEnemysBase(int x, int y) {
  bool isWhite =
      chessAPIService_->getField(x, y)._pieceColor == PieceColor::White;
  switchDialog_ = new SwitchPawnDialog(isWhite, x, y, this);
  connect(switchDialog_, &SwitchPawnDialog::pieceChosen, this,
          [=](int x, int y, PieceTypes piece) {
            chessAPIService_->switchToQueen(x, y, piece);
          });
  switchDialog_->setAttribute(Qt::WA_DeleteOnClose);
  switchDialog_->exec();
}
void OnlineChessWidget::onCheck() {
  //  QMessageBox::information(this, tr("Check"), QString("Check!"));
  qDebug() << "CHECK!!\n";
}

void OnlineChessWidget::onStartGame(int fixedPlayerNumber) {
  fixedPlayerNumber_ = fixedPlayerNumber;
  fixedOwnPieceColor_ = static_cast<PieceColor>(fixedPlayerNumber);
  fixedEnemyPieceColor_ = static_cast<PieceColor>(fixedPlayerNumber % 2 + 1);

  newGame();
  updateStatusLabel();

  qDebug() << "STARTING GAME";
}

void OnlineChessWidget::onRefreshTable() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      updateCell(i, j, chessAPIService_->getField(i, j));
    }
  }
  updateStatusLabel();
  qDebug() << "Refreshing Table";
}

void OnlineChessWidget::updateStatusLabel() {
  if (chessAPIService_->getCurrentPlayer() == fixedPlayerNumber_) {
    ui->statusLabel->setText("You may step...");
    updateTableEnabled(true);
  } else {
    ui->statusLabel->setText("Waiting for opponet to step...");
    updateTableEnabled(false);
  }
}

void OnlineChessWidget::updateTableEnabled(bool enabled) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      tableView_[i * 8 + j]->setEnabled(enabled);
    }
  }
}
