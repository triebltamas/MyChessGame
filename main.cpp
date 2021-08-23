#include "MyChessGame/View/ChessView.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  ChessView w;
  w.show();
  return a.exec();
}
