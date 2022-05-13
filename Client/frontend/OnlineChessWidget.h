#ifndef ONLINECHESSWIDGET_H
#define ONLINECHESSWIDGET_H

#include "ChessTableWidget.h"
#include "backend/ChessAPIService.h"
#include "backend/IChessModel.h"
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
  OnlineChessWidget(std::shared_ptr<ChessAPIService> chessAPIService,
                    IChessModel *model);
  ~OnlineChessWidget();

public slots:
  void onGameOver(int Player, int newElo, bool disconnected);
  void onStartGame(int fixedPlayerNumber);

private:
  void updateStatusLabel(int player);
  void updateTableEnabled(bool enable);

  ChessTableWidget *chessTable_;
  Ui::OnlineChessWidget *ui;
  std::shared_ptr<ChessAPIService> chessAPIService_;
  IChessModel *model_;
  int fixedPlayerNumber_ = -1;
};
#endif // ONLINECHESSWIDGET_H
