#ifndef CHESSVIEW_H
#define CHESSVIEW_H

#include "MyChessGame/Model/ChessModel.h"
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChessView;
}
QT_END_NAMESPACE

class ChessView : public QMainWindow {
  Q_OBJECT

public:
  ChessView(QWidget *parent = nullptr);
  ~ChessView();

private:
  void initUI();

  Ui::ChessView *ui;

  QMap<int, QPushButton *> _tableView;
  ChessModel *_model;
};
#endif // CHESSVIEW_H
