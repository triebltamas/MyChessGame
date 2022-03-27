#ifndef CHESSVIEW_H
#define CHESSVIEW_H

#include "ChessViewModel.h"
#include "SwitchPawnDialog.h"
#include <QLabel>
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
  void onCheck();
  void onPawnHasReachedEnemysBase(int x, int y);
  void onCellClicked(int x, int y);

private:
  void initUI();
  void newGame();
  void exit();
  void generateTable();
  void updateCell(int x, int y, ChessField field, bool initField = false);

  Ui::ChessView *ui;
  SwitchPawnDialog *switchDialog = nullptr;
  QMap<int, QPushButton *> _tableView;
  QLabel *currentPlayerLabel;
  ChessViewModel *_model;
  QPair<int, int> clickedCell_;
  bool green = false;
};
#endif // CHESSVIEW_H
