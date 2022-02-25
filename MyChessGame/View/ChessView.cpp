#include "ChessView.h"
#include "ui_ChessView.h"
#include <QMessageBox>

ChessView::ChessView(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ChessView), _model(new ChessModel) {
  ui->setupUi(this);
  _model = new ChessModel();
  connect(_model, &ChessModel::gameOver, this, &ChessView::onGameOver);
  connect(_model, &ChessModel::stepped, this, &ChessView::onStepped);
  connect(_model, &ChessModel::pawnHasReachedEnemysBase, this,
          &ChessView::onPawnHasReachedEnemysBase);
  connect(_model, &ChessModel::check, this, &ChessView::onCheck);
  connect(_model, &ChessModel::refreshTable, this, &ChessView::onRefreshTable);
  initUI();
}

ChessView::~ChessView() { delete ui; }

void ChessView::initUI() { newGame(); }

void ChessView::newGame() {
  _model->newGame();
  generateTable();
}

void ChessView::generateTable() {
  for (int i = 0; i < _tableView.size(); ++i) {
    for (int j = 0; j < _tableView.size(); ++j) {
      if (_tableView[i * 8 + j])
        delete _tableView[i * 8 + j];
    }
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      _tableView.insert(i * 8 + j, new QPushButton());
      ui->gridLayout->addWidget(_tableView[i * 8 + j], i, j);
      updateCell(i, j, _model->getField(i, j), true);

      connect(_tableView[i * 8 + j], &QPushButton::clicked, this,
              [this, i, j]() { onCellClicked(i, j); });
    }
  }
}

void ChessView::updateCell(int x, int y, ChessField field, bool initField) {
  if (initField) {
    switch (field._fieldColor) {
    case FieldColor::Black:
      _tableView[x * 8 + y]->setStyleSheet(
          "background-color: black; color: white;");
      break;
    case FieldColor::White:
      _tableView[x * 8 + y]->setStyleSheet(
          "background-color: white; color: black;");

      break;
    default:
      break;
    }
  }

  switch (field._pieceColor) {
  case PieceColor::Black:
    _tableView[x * 8 + y]->setText("B - ");
    break;
  case PieceColor::White:
    _tableView[x * 8 + y]->setText("W - ");
    break;
  case PieceColor::VoidColor:
    _tableView[x * 8 + y]->setText("");
    break;
  default:
    break;
  }

  switch (field._pieceType) {
  case PieceTypes::King:
    _tableView[x * 8 + y]->setText(_tableView[x * 8 + y]->text() + "King");
    break;
  case PieceTypes::Queen:
    _tableView[x * 8 + y]->setText(_tableView[x * 8 + y]->text() + "Queen");
    break;
  case PieceTypes::Bishup:
    _tableView[x * 8 + y]->setText(_tableView[x * 8 + y]->text() + "Bishup");
    break;
  case PieceTypes::Knight:
    _tableView[x * 8 + y]->setText(_tableView[x * 8 + y]->text() + "Knight");
    break;
  case PieceTypes::Rook:
    _tableView[x * 8 + y]->setText(_tableView[x * 8 + y]->text() + "Rook");
    break;
  case PieceTypes::Pawn:
    _tableView[x * 8 + y]->setText(_tableView[x * 8 + y]->text() + "Pawn");
    break;
  case PieceTypes::VoidType:
    _tableView[x * 8 + y]->setText("");
    break;
  default:
    break;
  }
}

void ChessView::onGameOver(int Player) {
  if (Player == 0) {
    QMessageBox::information(this, tr("Game over"), QString("Draw"));
  } else {
    QString msg = (Player == 1) ? QString("White won!") : QString("Black won!");
    QMessageBox::information(this, tr("Game over"), msg);
  }
  newGame();
}

void ChessView::onCellClicked(int x, int y) {
  if (_model->getField(x, y)._pieceColor == PieceColor::VoidColor)
    return;

  if (green) {
    if (x == clickedCell_.first && y == clickedCell_.second) {
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          updateCell(i, j, _model->getField(i, j), true);

      green = false;
    } else {
      if (_model->getField(x, y).highlighted)
        _model->stepPiece(clickedCell_.first, clickedCell_.second, x, y);
    }

  } else {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
        updateCell(i, j, _model->getField(i, j), true);

    auto cells = _model->possibleSteps(x, y);
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
void ChessView::onStepped(bool PieceKnockedDown) {}

void ChessView::onPawnHasReachedEnemysBase(int Pos) {}
void ChessView::onCheck() {}
void ChessView::onRefreshTable() {}
