#include "SwitchPawnDialog.h"
#include "ui_SwitchPawnDialog.h"

SwitchPawnDialog::SwitchPawnDialog(bool white, int x, int y, QWidget *parent)
    : QDialog(parent), x_(x), y_(y), ui(new Ui::SwitchPawnDialog),
      queenButton(new QPushButton(this)), rookButton(new QPushButton(this)),
      knightButton(new QPushButton(this)), bishupButton(new QPushButton(this)) {
  ui->setupUi(this);

  queenButton->setMinimumSize(QSize(100, 100));
  queenButton->setIconSize(QSize(100, 100));
  queenButton->setSizeIncrement(QSize(1, 1));

  rookButton->setMinimumSize(QSize(100, 100));
  rookButton->setIconSize(QSize(100, 100));
  rookButton->setSizeIncrement(QSize(1, 1));

  knightButton->setMinimumSize(QSize(100, 100));
  knightButton->setIconSize(QSize(100, 100));
  knightButton->setSizeIncrement(QSize(1, 1));

  bishupButton->setMinimumSize(QSize(100, 100));
  bishupButton->setIconSize(QSize(100, 100));
  bishupButton->setSizeIncrement(QSize(1, 1));
  queenButton->setStyleSheet(QString("QPushButton { background-color: rgb(255, 214, 173);}"));
  rookButton->setStyleSheet(QString("QPushButton { background-color: rgb(150, 82, 33);}"));
  knightButton->setStyleSheet(QString("QPushButton { background-color: rgb(255, 214, 173);}"));
  bishupButton->setStyleSheet(QString("QPushButton { background-color: rgb(150, 82, 33);}"));

  if (white) {
    queenButton->setIcon(QIcon(":/Application/QueenWhite"));
    rookButton->setIcon(QIcon(":/Application/RookWhite"));
    knightButton->setIcon(QIcon(":/Application/KnightWhite"));
    bishupButton->setIcon(QIcon(":/Application/BishupWhite"));
  } else {
    queenButton->setIcon(QIcon(":/Application/QueenBlack"));
    rookButton->setIcon(QIcon(":/Application/RookBlack"));
    knightButton->setIcon(QIcon(":/Application/KnightBlack"));
    bishupButton->setIcon(QIcon(":/Application/BishupBlack"));
  }
  ui->piecesLayout->addWidget(queenButton);
  ui->piecesLayout->addWidget(rookButton);
  ui->piecesLayout->addWidget(knightButton);
  ui->piecesLayout->addWidget(bishupButton);

  connect(queenButton, &QPushButton::clicked, this, [=](auto) {
    selection_ = PieceTypes::Queen;
    selectionChanged();
  });
  connect(rookButton, &QPushButton::clicked, this, [=](auto) {
    selection_ = PieceTypes::Rook;
    selectionChanged();
  });
  connect(knightButton, &QPushButton::clicked, this, [=](auto) {
    selection_ = PieceTypes::Knight;
    selectionChanged();
  });
  connect(bishupButton, &QPushButton::clicked, this, [=](auto) {
    selection_ = PieceTypes::Bishup;
    selectionChanged();
  });
  connect(ui->choosePushButton, &QPushButton::clicked, this,
          [this](auto) { this->close(); });
  selectionChanged();
}

void SwitchPawnDialog::selectionChanged() {
    queenButton->setStyleSheet(QString("QPushButton { background-color: rgb(255, 214, 173);}"));
    rookButton->setStyleSheet(QString("QPushButton { background-color: rgb(150, 82, 33);}"));
    knightButton->setStyleSheet(QString("QPushButton { background-color: rgb(255, 214, 173);}"));
    bishupButton->setStyleSheet(QString("QPushButton { background-color: rgb(150, 82, 33);}"));

    switch (selection_) {
    case PieceTypes::Queen:
      queenButton->setStyleSheet("QPushButton { background-color: rgb(0, 184, 44);}");

      break;
    case PieceTypes::Rook:
      rookButton->setStyleSheet("QPushButton { background-color: rgb(0, 140, 33);}");

      break;
    case PieceTypes::Knight:
      knightButton->setStyleSheet("QPushButton { background-color: rgb(0, 184, 44);}");

      break;
    case PieceTypes::Bishup:
      bishupButton->setStyleSheet("QPushButton { background-color: rgb(0, 140, 33);}");

      break;
    default:
      break;
    }
}

SwitchPawnDialog::~SwitchPawnDialog() { emit pieceChosen(x_, y_, selection_); }
