#ifndef CHESSMODEL_H
#define CHESSMODEL_H
#include "ChessField.h"
#include "IChessModel.h"
#include <QJsonObject>
#include <QList>
#include <QPair>

class ChessModel : public IChessModel {
public:
  ChessModel();
  //  ChessModel(int fixedPlayerNumber);

  QList<QPair<int, int>> possibleSteps(int x, int y,
                                       bool includeDefendedPieces = false,
                                       bool attack = false,
                                       bool newTable = false) override;

  void newGame() override;
  void stepPiece(int from_x, int from_y, int to_x, int to_y) override;
  ChessField getField(int x, int y) override;
  void setHighlighted(int x, int y, bool highlight) override;
  void switchToQueen(int x, int y, PieceTypes switchTo) override;
  int getCurrentPlayer() override;
  bool isMyPiece(int x, int y) override;

  QJsonObject serializeField(int x, int y);
  void deSerializeField(QJsonObject fieldJson, int x, int y);

private:
  bool checkGameOver();
  bool stepCausesSelfCheck(int from_x, int from_y, int to_x, int to_y,
                           bool attack);
  bool isSamePieceColor(int x, int y, PieceColor color, bool newTable,
                        bool includeDefendedPieces);

  QList<QPair<int, int>>
  possibleStepsForKing(int x, int y, PieceColor color,
                       bool includeDefendedPieces = false, bool attack = false,
                       bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForQueen(int x, int y, PieceColor color,
                        bool includeDefendedPieces = false, bool attack = false,
                        bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForRook(int x, int y, PieceColor color,
                       bool includeDefendedPieces = false, bool attack = false,
                       bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForBishup(int x, int y, PieceColor color,
                         bool includeDefendedPieces = false,
                         bool attack = false, bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForKnight(int x, int y, PieceColor color,
                         bool includeDefendedPieces = false,
                         bool attack = false, bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForPawn(int x, int y, PieceColor color,
                       bool includeDefendedPieces = false, bool attack = false,
                       bool newTable = false);

  int N_;
  int currentPlayer_; // 1 or 2 or 0 if draw
                      //  int fixedPlayerNumber_;
  bool isChecked = false;
  ChessField **chessTable_ = nullptr;
  ChessField **newTable_ = nullptr;
};

#endif // CHESSMODEL_H
