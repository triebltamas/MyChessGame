#ifndef LOCALCHESSWIDGET_H
#define LOCALCHESSWIDGET_H

#include "ChessTableWidget.h"
#include "backend/ChessModel.h"
#include <QWidget>
#include <memory>

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
  void onGameOver(int Player, int /*newElo*/, bool /*disconnected*/);
  void onCurrentPlayerChanged(int player);

private:
  bool enableFENImport_ = true;
  Ui::LocalChessWidget *ui;
  ChessTableWidget *chessTable;
  std::shared_ptr<ChessModel> model_;
};

#endif // LOCALCHESSWIDGET_H
