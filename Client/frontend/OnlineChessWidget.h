#ifndef ONLINECHESSWIDGET_H
#define ONLINECHESSWIDGET_H

#include "ChessTableWidget.h"
#include "common/ChessAPIService.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class OnlineChessWidget;
}
QT_END_NAMESPACE

class OnlineChessWidget : public QWidget {
  Q_OBJECT

public:
  OnlineChessWidget(std::shared_ptr<ChessAPIService> chessAPIService);
  ~OnlineChessWidget();

public slots:
  void onGameOver(int Player, int newElo);
  void onStartGame(int fixedPlayerNumber);

private:
  void updateStatusLabel(int player);
  void updateTableEnabled(bool enable);

  ChessTableWidget *chessTable_;
  Ui::OnlineChessWidget *ui;
  std::shared_ptr<ChessAPIService> chessAPIService_;
  int fixedPlayerNumber_ = -1;
};
#endif // ONLINECHESSWIDGET_H
