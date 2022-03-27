#include "ChessView.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/Application/Resources/GameIcon.ico"));
  ChessView w;
  w.show();
  return a.exec();
}
