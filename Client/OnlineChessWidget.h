#ifndef ONLINECHESSWIDGET_H
#define ONLINECHESSWIDGET_H

#include "ChessAPIService.h"
#include "SwitchPawnDialog.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class OnlineChessWidget;
}
QT_END_NAMESPACE

class OnlineChessWidget : public QWidget {
  Q_OBJECT

public:
  OnlineChessWidget(ChessAPIService *chessAPIService);
  ~OnlineChessWidget();

public slots:
  void onGameOver(int Player, int newElo);
  void onCheck();
  void onRefreshTable();
  void onStartGame(int fixedPlayerNumber);
  void onPawnHasReachedEnemysBase(int x, int y);
  void onCellClicked(int x, int y);

private:
  void newGame();
  void exit();
  void generateTable();
  void updateCell(int x, int y, ChessField field, bool initField = false);
  void paintCell(int x, int y, QString rgbWhite, QString rgbBlack);
  void updateStatusLabel();
  void updateTableEnabled(bool enable);

  Ui::OnlineChessWidget *ui;
  SwitchPawnDialog *switchDialog_ = nullptr;
  QMap<int, QPushButton *> tableView_;
  ChessAPIService *chessAPIService_;
  QPair<int, int> clickedCell_;
  bool green_ = false;
  int fixedPlayerNumber_ = -1;
  PieceColor fixedOwnPieceColor_ = PieceColor::VoidColor;
  PieceColor fixedEnemyPieceColor_ = PieceColor::VoidColor;
};
#endif // ONLINECHESSWIDGET_H
