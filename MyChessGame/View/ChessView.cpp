#include "ChessView.h"
#include "ui_ChessView.h"

ChessView::ChessView(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ChessView), _model(new ChessModel) {
  ui->setupUi(this);
  initUI();
}

ChessView::~ChessView() { delete ui; }

void ChessView::initUI() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      _tableView.insert(i * 8 + j, new QPushButton());
      ui->gridLayout->addWidget(_tableView[i * 8 + j], i, j);
    }
  }
}
