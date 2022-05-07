#ifndef CHESSMODELTEST_H
#define CHESSMODELTEST_H

#include "Client/backend/ChessModel.h"
#include <QObject>
#include <QTest>

class ChessModelTest : public QObject {
  Q_OBJECT
private:
  ChessModel *model = nullptr;
  bool isChecked = false;
  bool isMate = false;

private slots:
  void initTestCase();
  void init();
  void cleanupTestCase();

  void possibleStepsTest();
  void anPassantTest();
  void castlingTest();
  void checkTest();
  void gameOverTest();
  void possibleStepsBug1();
};

#endif // CHESSMODELTEST_H
