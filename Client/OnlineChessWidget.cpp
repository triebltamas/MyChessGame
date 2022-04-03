#include "OnlineChessWidget.h"
#include "ui_OnlineChessWidget.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

OnlineChessWidget::OnlineChessWidget(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::OnlineChessWidget),
      _model(new ChessAPIService) {
  ui->setupUi(this);
  connect(_model, &ChessAPIService::connected, this,
          &OnlineChessWidget::onConnected);
  connect(_model, &ChessAPIService::startGame, this,
          &OnlineChessWidget::onStartGame);
  connect(_model, &ChessAPIService::refreshTable, this,
          &OnlineChessWidget::onRefreshTable);
  connect(_model, &ChessAPIService::gameOver, this,
          &OnlineChessWidget::onGameOver);
  connect(_model, &ChessAPIService::pawnHasReachedEnemysBase, this,
          &OnlineChessWidget::onPawnHasReachedEnemysBase);
  connect(_model, &ChessAPIService::check, this, &OnlineChessWidget::onCheck);
  connect(ui->actionNewGame, &QAction::triggered, this,
          &OnlineChessWidget::newGame);
  connect(ui->actionExit, &QAction::triggered, this, &OnlineChessWidget::exit);
  initUI();
}

OnlineChessWidget::~OnlineChessWidget() { delete ui; }

void OnlineChessWidget::initUI() { newGame(); }

void OnlineChessWidget::newGame() {
  _model->newGame();
  generateTable();
}

void OnlineChessWidget::generateTable() {
  int i = 0;
  for (int j = 0; j < 8 && i < 8; ++j && ++i) {
    if (_tableView[i * 8 + j] != nullptr)
      delete _tableView[i * 8 + j];
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      _tableView.insert(i * 8 + j, new QPushButton());
      _tableView[i * 8 + j]->setMinimumSize(QSize(100, 100));
      _tableView[i * 8 + j]->setSizeIncrement(QSize(1, 1));
      _tableView[i * 8 + j]->setIconSize(QSize(50, 50));
      _tableView[i * 8 + j]->setStyleSheet("text-align: center;");
      //      _tableView[i * 8 + j]->setFlat(true);

      ui->gridLayout->addWidget(_tableView[i * 8 + j], i, j);
      updateCell(i, j, _model->getField(i, j), true);

      connect(_tableView[i * 8 + j], &QPushButton::clicked, this,
              [this, i, j]() { onCellClicked(i, j); });
    }
  }
}

void OnlineChessWidget::updateCell(int x, int y, ChessField field,
                                   bool initField) {
  if (initField) {
    switch (field._fieldColor) {
    case FieldColor::Black:
      _tableView[x * 8 + y]->setStyleSheet(
          "background-color: brown; color: white;");
      break;
    case FieldColor::White:
      _tableView[x * 8 + y]->setStyleSheet(
          "background-color: white; color: black;");

      break;
    default:
      break;
    }
  }

  switch (field._pieceType) {
  case PieceTypes::King:
    _tableView[x * 8 + y]->setText("");
    if (field._pieceColor == PieceColor::White)
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/KingWhite"));
    else
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/KingBlack"));
    break;
  case PieceTypes::Queen:
    if (field._pieceColor == PieceColor::White)
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/QueenWhite"));
    else
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/QueenBlack"));
    break;
  case PieceTypes::Bishup:
    if (field._pieceColor == PieceColor::White)
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/BishupWhite"));
    else
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/BishupBlack"));
    break;
  case PieceTypes::Knight:
    if (field._pieceColor == PieceColor::White)
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/KnightWhite"));
    else
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/KnightBlack"));
    break;
  case PieceTypes::Rook:
    if (field._pieceColor == PieceColor::White)
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/RookWhite"));
    else
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/RookBlack"));
    break;
  case PieceTypes::Pawn:
    if (field._pieceColor == PieceColor::White)
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/PawnWhite"));
    else
      _tableView[x * 8 + y]->setIcon(QIcon(":/Application/PawnBlack"));
    break;
  case PieceTypes::VoidType:
    _tableView[x * 8 + y]->setIcon(QIcon());
    break;
  default:
    break;
  }
}

void OnlineChessWidget::onGameOver(int Player) {
  if (Player == 0) {
    QMessageBox::information(this, tr("Game over"), QString("Draw"));
  } else {
    QString msg = (Player == 1) ? QString("White won!") : QString("Black won!");
    QMessageBox::information(this, tr("Game over"), msg);
  }
  newGame();
}

void OnlineChessWidget::onCellClicked(int x, int y) {
  if (_model->getField(x, y)._pieceColor == PieceColor::VoidColor &&
      !_model->getField(x, y).highlighted)
    return;

  if (green) {
    if (x == clickedCell_.first && y == clickedCell_.second) {
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          updateCell(i, j, _model->getField(i, j), true);
          _model->setHighlighted(i, j, false);
        }
      }

      green = false;
    } else {
      if (_model->getField(x, y).highlighted) {
        _model->stepPiece(clickedCell_.first, clickedCell_.second, x, y);

        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            updateCell(i, j, _model->getField(i, j), true);
            _model->setHighlighted(i, j, false);
          }
        }
        green = false;
      } else {
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            updateCell(i, j, _model->getField(i, j), true);
            _model->setHighlighted(i, j, false);
          }
        }

        auto cells = _model->possibleSteps(x, y, false, true, false);
        if (!cells.empty())
          cells.append(QPair<int, int>(x, y));

        for (auto cell : cells) {
          _tableView[cell.first * 8 + cell.second]->setStyleSheet(
              "background-color: green");

          _model->setHighlighted(cell.first, cell.second, true);
        }

        green = true;

        clickedCell_.first = x;
        clickedCell_.second = y;
      }
    }

  } else {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        updateCell(i, j, _model->getField(i, j), true);
      }
    }

    auto cells = _model->possibleSteps(x, y, false, true, false);
    if (!cells.empty())
      cells.append(QPair<int, int>(x, y));

    for (auto cell : cells) {
      _tableView[cell.first * 8 + cell.second]->setStyleSheet(
          "background-color: green");

      _model->setHighlighted(cell.first, cell.second, true);
    }

    green = true;

    clickedCell_.first = x;
    clickedCell_.second = y;
  }
}

void OnlineChessWidget::onPawnHasReachedEnemysBase(int x, int y) {
  bool isWhite = _model->getField(x, y)._pieceColor == PieceColor::White;
  switchDialog = new SwitchPawnDialog(isWhite, x, y, this);
  connect(switchDialog, &SwitchPawnDialog::pieceChosen, this,
          [=](int x, int y, PieceTypes piece) {
            _model->switchToQueen(x, y, piece);
          });
  switchDialog->setAttribute(Qt::WA_DeleteOnClose);
  switchDialog->exec();
}
void OnlineChessWidget::onCheck() {
  //  QMessageBox::information(this, tr("Check"), QString("Check!"));
  qDebug() << "CHECK!!\n";
}

void OnlineChessWidget::onConnected(int fixedPlayerNumber) {
  fixedPlayerNumber_ = fixedPlayerNumber;
  // TODO waiting screen
  qDebug() << "CONNECTED!!WAITING FOR OTHER PLAYER\n";
}

void OnlineChessWidget::onStartGame() {
  // TODO get gametable widget and start the game
  _model->newGame();
  qDebug() << "STARTING GAME";
}

void OnlineChessWidget::onRefreshTable() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      updateCell(i, j, _model->getField(i, j));
    }
  }
  qDebug() << "Refreshing Table";
}

void OnlineChessWidget::exit() { this->close(); }
