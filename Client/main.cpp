#include "ChessMainWindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/Application/ClientIcon"));
  ChessMainWindow w;
  w.show();
  return a.exec();
}
