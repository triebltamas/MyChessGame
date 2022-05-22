#ifndef LOCALCHESSWIDGET_H
#define LOCALCHESSWIDGET_H

#include "ChessTableWidget.h"
#include "backend/IChessModel.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class LocalChessWidget;
}
QT_END_NAMESPACE

class LocalChessWidget : public QWidget {
  Q_OBJECT

public:
  LocalChessWidget(IChessModel *model);
  ~LocalChessWidget();

public slots:
  void onGameOver(int Player, int /*newElo*/, bool /*disconnected*/);
  void onCurrentPlayerChanged(int player);

private:
  bool enableFENImport_ = true;
  Ui::LocalChessWidget *ui;
  ChessTableWidget *chessTable;
  IChessModel *model_;
};

#endif // LOCALCHESSWIDGET_H
