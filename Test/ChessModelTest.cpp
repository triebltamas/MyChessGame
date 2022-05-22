#include "ChessModelTest.h"

void ChessModelTest::initTestCase() {
  model = new ChessModel();
  model->newGame();
  connect(
      model, &ChessModel::check, this, [this]() { isChecked = true; },
      Qt::DirectConnection);
  connect(
      model, &ChessModel::gameOver, this, [this]() { isMate = true; },
      Qt::DirectConnection);
}

void ChessModelTest::init() {
  if (model == nullptr)
    return;

  model->newGame();
  isChecked = false;
  isMate = false;
}

void ChessModelTest::possibleStepsTest() {
  QString fen = "1nbqkbnr/r1p2pp1/3Q4/1P2P1B1/1R2p2p/8/1PP2PPP/1N2KBNR w";
  model->importFEN(fen);

  auto whitePawnSteps = model->possibleSteps(6, 2, false, true, false);   // 2
  auto whiteRookSteps = model->possibleSteps(4, 1, false, true, false);   // 5
  auto whiteBishupSteps = model->possibleSteps(3, 6, false, true, false); // 9
  auto whiteKnightSteps = model->possibleSteps(7, 1, false, true, false); // 3
  auto whiteQueenSteps = model->possibleSteps(2, 3, false, true, false);  // 18
  auto whiteKingSteps = model->possibleSteps(7, 4, false, true, false);   // 3

  QString fen2 = "1nbqkbnr/r1p2pp1/3Q4/1P2P1B1/1R2p2p/8/1PP2PPP/1N2KBNR b";
  model->importFEN(fen2);

  auto blackPawnSteps = model->possibleSteps(1, 2, false, true, false);   // 3
  auto blackRookSteps = model->possibleSteps(1, 0, false, true, false);   // 8
  auto blackBishupSteps = model->possibleSteps(0, 5, false, true, false); // 2
  auto blackKnightSteps = model->possibleSteps(0, 1, false, true, false); // 3
  auto blackQueenSteps = model->possibleSteps(0, 3, false, true, false);  // 5
  auto blackKingSteps = model->possibleSteps(0, 4, false, true, false);   // 0

  QCOMPARE(whitePawnSteps.length(), 2);
  QCOMPARE(whiteRookSteps.length(), 5);
  QCOMPARE(whiteBishupSteps.length(), 9);
  QCOMPARE(whiteKnightSteps.length(), 3);
  QCOMPARE(whiteQueenSteps.length(), 18);
  QCOMPARE(whiteKingSteps.length(), 3);

  QCOMPARE(blackPawnSteps.length(), 3);
  QCOMPARE(blackRookSteps.length(), 8);
  QCOMPARE(blackBishupSteps.length(), 2);
  QCOMPARE(blackKnightSteps.length(), 3);
  QCOMPARE(blackQueenSteps.length(), 5);
  QCOMPARE(blackKingSteps.length(), 0);
}

void ChessModelTest::anPassantTest() {
  QString fen = "1nbqkbnr/r1p2pp1/3Q4/1P2P1B1/1R2p2p/8/1PP2PPP/1N2KBNR b";
  model->importFEN(fen);
  model->stepPiece(1, 5, 3, 5);

  auto whitePawnSteps = model->possibleSteps(3, 4, false, true, false); // 2
  QCOMPARE(whitePawnSteps.length(), 2);
  QVERIFY(whitePawnSteps.contains(QPair<int, int>(2, 5)));

  model->stepPiece(3, 4, 2, 5);
  auto pawnField = model->getField(2, 5);
  auto voidFiled1 = model->getField(3, 5);
  auto voidField2 = model->getField(3, 4);

  QVERIFY(pawnField._pieceType == PieceTypes::Pawn);
  QVERIFY(voidFiled1._pieceType == PieceTypes::VoidType);
  QVERIFY(voidField2._pieceType == PieceTypes::VoidType);
}

void ChessModelTest::castlingTest() {
  QString fen = "r3kbnr/pppqpppp/n7/3p1b2/3P1B2/N7/PPPQPPPP/R3KBNR w";
  model->importFEN(fen);

  auto whiteKingSteps = model->possibleSteps(7, 4, false, true, false); // 2
  QCOMPARE(whiteKingSteps.length(), 2);
  QVERIFY(whiteKingSteps.contains(QPair<int, int>(7, 2)));

  model->stepPiece(7, 4, 7, 2);
  auto kingField = model->getField(7, 2);
  auto rookField = model->getField(7, 3);
  auto voidFiled1 = model->getField(7, 4);
  auto voidField2 = model->getField(7, 0);

  QVERIFY(kingField._pieceType == PieceTypes::King);
  QVERIFY(rookField._pieceType == PieceTypes::Rook);
  QVERIFY(voidFiled1._pieceType == PieceTypes::VoidType);
  QVERIFY(voidField2._pieceType == PieceTypes::VoidType);
}

void ChessModelTest::checkTest() {
  QString fen = "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w";
  model->importFEN(fen);

  QVERIFY(!isChecked);
  QVERIFY(!isMate);
  model->stepPiece(7, 5, 3, 1);
  QVERIFY(isChecked);
  QVERIFY(!isMate);
}

void ChessModelTest::gameOverTest() {
  QString fen = "rnbqkbnr/1pp2ppp/8/p3P1B1/4p3/8/PPP2PPP/RN1QKBNR w";
  model->importFEN(fen);

  QVERIFY(!isChecked);
  QVERIFY(!isMate);
  model->stepPiece(7, 3, 0, 3);
  QVERIFY(isChecked);
  QVERIFY(isMate);
}

void ChessModelTest::cleanupTestCase() {
  if (model != nullptr)
    delete model;
}

void ChessModelTest::possibleStepsBug1() {
  QString fen = "8/4Q1kp/5R2/6p1/5r2/7P/6PK/5q2 b";
  model->importFEN(fen);
  auto blackRookSteps = model->possibleSteps(4, 5, false, true, false);
  QCOMPARE(blackRookSteps.length(), 0);
}

void ChessModelTest::checkForDrawBug1() {
  QString fen = "2Q2bnr/4p1pq/5pkr/7p/2P4P/8/PP1PPPP1/RNB1KBNR w";
  model->importFEN(fen);
  model->stepPiece(0, 2, 2, 4);
  QVERIFY(isMate);
  QCOMPARE(model->getCurrentPlayer(), 0);
}
QTEST_MAIN(ChessModelTest)
#include "ChessModelTest.moc"
