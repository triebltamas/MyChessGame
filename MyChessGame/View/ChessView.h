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

public slots:
  void onGameOver(int Player);
  void onStepped(bool PieceKnockedDown);
  void onCheck();
  void onPawnHasReachedEnemysBase(int Pos);
  void onRefreshTable();
  void onCellClicked(int x, int y);

private:
  void initUI();
  void newGame();
  void generateTable();
  void updateCell(int x, int y, ChessField field, bool initField = false);

  Ui::ChessView *ui;

  QMap<int, QPushButton *> _tableView;
  ChessModel *_model;
  QPair<int, int> clickedCell_;
  bool green = false;
};
#endif // CHESSVIEW_H
