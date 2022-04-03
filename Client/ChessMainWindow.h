#ifndef CHESSMAINWINDOW_H
#define CHESSMAINWINDOW_H

#include "ChessAPIService.h"
#include "ChessField.h"
#include "SwitchPawnDialog.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChessMainWindow;
}
QT_END_NAMESPACE

class ChessMainWindow : public QMainWindow {
  Q_OBJECT

public:
  ChessMainWindow(QWidget *parent = nullptr);
  ~ChessMainWindow();

public slots:
  void onGameOver(int Player);
  void onCheck();
  void onRefreshTable();
  void onStartGame();
  void onConnected(int fixedPlayerNumber);
  void onPawnHasReachedEnemysBase(int x, int y);
  void onCellClicked(int x, int y);

private:
  void initUI();
  void newGame();
  void exit();
  void generateTable();
  void updateCell(int x, int y, ChessField field, bool initField = false);

  Ui::ChessMainWindow *ui;
  SwitchPawnDialog *switchDialog = nullptr;
  QMap<int, QPushButton *> _tableView;
  QLabel *currentPlayerLabel;
  ChessAPIService *_model;
  QPair<int, int> clickedCell_;
  bool green = false;
  int fixedPlayerNumber_ = -1;
};
#endif // CHESSMAINWINDOW_H
