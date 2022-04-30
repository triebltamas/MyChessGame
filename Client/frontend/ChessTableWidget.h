#ifndef CHESSTABLEWIDGET_H
#define CHESSTABLEWIDGET_H

#include "SwitchPawnDialog.h"
#include "backend/IChessModel.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChessTableWidget;
}
QT_END_NAMESPACE

class ChessTableWidget : public QWidget {
  Q_OBJECT

public:
  ChessTableWidget(std::shared_ptr<IChessModel> model, QWidget *parent);
  ~ChessTableWidget();
  void newGame();
  void updateTableEnabled(bool enabled);

signals:
  void currentPlayerChanged(int player);

public slots:
  void onCheck();
  void onPawnHasReachedEnemysBase(int x, int y);
  void onCellClicked(int x, int y);
  void onRefreshTable();

private:
  void generateTable();
  void updateCell(int x, int y, ChessField field, bool initField = false);
  void paintCell(int x, int y, QString rgbWhite, QString rgbBlack);

  Ui::ChessTableWidget *ui;
  SwitchPawnDialog *switchDialog_ = nullptr;
  QMap<int, QPushButton *> tableView_;
  std::shared_ptr<IChessModel> model_;
  QPair<int, int> clickedCell_;
  bool green_ = false;
};
#endif // CHESSTABLEWIDGET_H
