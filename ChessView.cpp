#include "ChessView.h"
#include "ui_ChessView.h"

ChessView::ChessView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChessView)
{
    ui->setupUi(this);
}

ChessView::~ChessView()
{
    delete ui;
}

