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
  void paintCell(int x, int y, QString rgbWhite, QString rgbBlack);

  Ui::LocalChessWidget *ui;
  SwitchPawnDialog *switchDialog_ = nullptr;
  QMap<int, QPushButton *> tableView_;
  ChessModel *model_;
  QPair<int, int> clickedCell_;
  bool green_ = false;
};
#endif // LOCALCHESSWIDGET_H
