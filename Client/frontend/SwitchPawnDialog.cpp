#include "SwitchPawnDialog.h"
#include "ui_SwitchPawnDialog.h"

SwitchPawnDialog::SwitchPawnDialog(bool white, int x, int y, QWidget *parent)
    : QDialog(parent), x_(x), y_(y), ui(new Ui::SwitchPawnDialog),
      queenButton(new QPushButton(this)), rookButton(new QPushButton(this)),
      knightButton(new QPushButton(this)), bishupButton(new QPushButton(this)) {
  ui->setupUi(this);

  queenButton->setMinimumSize(QSize(100, 100));
  queenButton->setSizeIncrement(QSize(1, 1));
  rookButton->setMinimumSize(QSize(100, 100));
  rookButton->setSizeIncrement(QSize(1, 1));
  knightButton->setMinimumSize(QSize(100, 100));
  knightButton->setSizeIncrement(QSize(1, 1));
  bishupButton->setMinimumSize(QSize(100, 100));
  bishupButton->setSizeIncrement(QSize(1, 1));

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
  // HIghlight selected Button, and dehighlight other buttons
  auto makeWhite = [](QList<QPushButton *> buttons) {
    for (auto button : buttons)
      button->setStyleSheet("background-color: white;");
  };

  switch (selection_) {
  case PieceTypes::Queen:
    queenButton->setStyleSheet("background-color: green;");
    makeWhite({rookButton, knightButton, bishupButton});

    break;
  case PieceTypes::Rook:
    rookButton->setStyleSheet("background-color: green;");
    makeWhite({queenButton, knightButton, bishupButton});

    break;
  case PieceTypes::Knight:
    knightButton->setStyleSheet("background-color: green;");
    makeWhite({queenButton, rookButton, bishupButton});

    break;
  case PieceTypes::Bishup:
    bishupButton->setStyleSheet("background-color: green;");
    makeWhite({queenButton, rookButton, knightButton});

    break;
  default:
    break;
  }
}

SwitchPawnDialog::~SwitchPawnDialog() { emit pieceChosen(x_, y_, selection_); }
