#ifndef LOCALCHESSWIDGET_H
#define LOCALCHESSWIDGET_H

#include "ChessModel.h"
#include "SwitchPawnDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class LocalChessWidget;
}
QT_END_NAMESPACE

class LocalChessWidget : public QWidget {
  Q_OBJECT

public:
  LocalChessWidget();
  ~LocalChessWidget();

public slots:
  void onGameOver(int Player);
  void onCheck();
  void onPawnHasReachedEnemysBase(int x, int y);
  void onCellClicked(int x, int y);

private:
  void newGame();
  void generateTable();
  void updateCell(int x, int y, ChessField field, bool initField = false);

  Ui::LocalChessWidget *ui;
  SwitchPawnDialog *switchDialog = nullptr;
  QMap<int, QPushButton *> _tableView;
  QLabel *currentPlayerLabel;
  ChessModel *_model;
  QPair<int, int> clickedCell_;
  bool green = false;
};
#endif // LOCALCHESSWIDGET_H
