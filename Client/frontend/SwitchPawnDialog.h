#ifndef SwitchPawnDialog_H
#define SwitchPawnDialog_H

#include "common/ChessField.h"
#include <QDialog>
#include <QObject>
#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class SwitchPawnDialog;
}
QT_END_NAMESPACE

class SwitchPawnDialog : public QDialog {
  Q_OBJECT
public:
  SwitchPawnDialog(bool white, int x, int y, QWidget *parent = nullptr);
  ~SwitchPawnDialog();

signals:
  void pieceChosen(int x, int y, PieceTypes piece);

private:
  void selectionChanged();
  Ui::SwitchPawnDialog *ui;

  QPushButton *queenButton = nullptr;
  QPushButton *rookButton = nullptr;
  QPushButton *knightButton = nullptr;
  QPushButton *bishupButton = nullptr;

  PieceTypes selection_ = PieceTypes::Queen;
  int x_;
  int y_;
};

#endif // SwitchPawnDialog_H
