#include "ChessTableWidget.h"
#include "HoverEventFilter.h"
#include "ui_ChessTableWidget.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

ChessTableWidget::ChessTableWidget(std::shared_ptr<IChessModel> model,
                                   QWidget *parent)
    : QWidget(parent), model_(model), ui(new Ui::ChessTableWidget) {
  ui->setupUi(this);
  connect(model_.get(), &IChessModel::pawnHasReachedEnemysBase, this,
          &ChessTableWidget::onPawnHasReachedEnemysBase);
  connect(model_.get(), &IChessModel::check, this, &ChessTableWidget::onCheck);
  connect(model_.get(), &IChessModel::refreshTable, this,
          &ChessTableWidget::onRefreshTable);
}

ChessTableWidget::~ChessTableWidget() { delete ui; }

void ChessTableWidget::newGame() {
  model_->newGame();
  generateTable();
}

void ChessTableWidget::generateTable() {
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
      tableView_[i * 8 + j]->setFocusPolicy(Qt::NoFocus);

      ui->gridLayout->addWidget(tableView_[i * 8 + j], i, j);
      updateCell(i, j, model_->getField(i, j), true);

      connect(tableView_[i * 8 + j], &QPushButton::clicked, this,
              [this, i, j]() { onCellClicked(i, j); });
    }
  }
  ui->centralWidget->setStyleSheet(
      "QWidget {background-color: rgb(255, 206, 112);}");
  ui->centralWidget->setSizeIncrement(1, 1);
}

void ChessTableWidget::updateCell(int x, int y, ChessField field,
                                  bool initField) {
  if (field.isLastStep)
    paintCell(x, y, "rgb(246, 246, 105)", "rgb(242, 242, 53)");
  else
    paintCell(x, y, "rgb(255, 214, 173)", "rgb(150, 82, 33)");

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

void ChessTableWidget::paintCell(int x, int y, QString rgbWhite,
                                 QString rgbBlack) {
  if (model_->getField(x, y)._fieldColor == FieldColor::White) {
    tableView_[x * 8 + y]->setStyleSheet(QString(
        "QPushButton { background-color: " + rgbWhite + "; border: 0px;}"));
  } else {
    tableView_[x * 8 + y]->setStyleSheet(QString(
        "QPushButton { background-color: " + rgbBlack + "; border: 0px;}"));
  }
}

void ChessTableWidget::onCellClicked(int x, int y) {
  if (model_->getField(x, y)._pieceColor == PieceColor::VoidColor &&
      !model_->getField(x, y).highlighted)
    return;

  if (green_) {
    if (x == clickedCell_.first && y == clickedCell_.second) {
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          updateCell(i, j, model_->getField(i, j), true);
          model_->setHighlighted(i, j, false);
        }
      }

      green_ = false;
    } else {
      if (model_->getField(x, y).highlighted) {
        model_->stepPiece(clickedCell_.first, clickedCell_.second, x, y);

        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            updateCell(i, j, model_->getField(i, j), true);
            model_->setHighlighted(i, j, false);
          }
        }
        green_ = false;
        emit currentPlayerChanged(model_->getCurrentPlayer());
      } else {
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            updateCell(i, j, model_->getField(i, j), true);
            model_->setHighlighted(i, j, false);
          }
        }

        if (!model_->isMyPiece(x, y))
          return;

        auto cells = model_->possibleSteps(x, y, false, true, false);
        if (!cells.empty())
          cells.append(QPair<int, int>(x, y));

        for (auto cell : cells) {
          paintCell(cell.first, cell.second, "rgb(0, 184, 44)",
                    "rgb(0, 140, 33)");
          model_->setHighlighted(cell.first, cell.second, true);
        }

        green_ = true;

        clickedCell_.first = x;
        clickedCell_.second = y;
      }
    }

  } else {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        updateCell(i, j, model_->getField(i, j), true);
      }
    }
    if (!model_->isMyPiece(x, y))
      return;

    auto cells = model_->possibleSteps(x, y, false, true, false);
    if (!cells.empty())
      cells.append(QPair<int, int>(x, y));

    for (auto cell : cells) {
      paintCell(cell.first, cell.second, "rgb(0, 184, 44)", "rgb(0, 140, 33)");
      model_->setHighlighted(cell.first, cell.second, true);
    }

    green_ = true;

    clickedCell_.first = x;
    clickedCell_.second = y;
  }
}

void ChessTableWidget::onPawnHasReachedEnemysBase(int x, int y) {
  bool isWhite = model_->getField(x, y)._pieceColor == PieceColor::White;
  switchDialog_ = new SwitchPawnDialog(isWhite, x, y, this);
  connect(switchDialog_, &SwitchPawnDialog::pieceChosen, this,
          [=](int x, int y, PieceTypes piece) {
            model_->switchToQueen(x, y, piece);
          });
  switchDialog_->setAttribute(Qt::WA_DeleteOnClose);
  switchDialog_->exec();
}
void ChessTableWidget::onCheck() { qDebug() << "CHECK!!\n"; }

void ChessTableWidget::onRefreshTable() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      updateCell(i, j, model_->getField(i, j));
    }
  }
  emit currentPlayerChanged(model_->getCurrentPlayer());
}

void ChessTableWidget::updateTableEnabled(bool enabled) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      tableView_[i * 8 + j]->setEnabled(enabled);
    }
  }
}
