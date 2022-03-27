#include "ChessServer.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  ChessServer s;
  return a.exec();
}
