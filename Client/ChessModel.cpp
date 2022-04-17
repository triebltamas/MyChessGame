#include "ChessModel.h"
#include <QDebug>

ChessModel::ChessModel() : N_(8), currentPlayer_(1) {
  chessTable_ = new ChessField *[8];
  for (int i = 0; i < N_; i++)
    chessTable_[i] = new ChessField[8];

  newTable_ = new ChessField *[8];
  for (int i = 0; i < N_; i++)
    newTable_[i] = new ChessField[8];
}

void ChessModel::newGame() {
  currentPlayer_ = 1;
  if (chessTable_ == nullptr) {
    qWarning() << "CHESSTABLE IS NULLPOINTER!";
    return;
  }

  // Setting up the table's piecetypes
  chessTable_[0][4]._pieceType = PieceTypes::King;
  chessTable_[7][4]._pieceType = PieceTypes::King;
  chessTable_[0][3]._pieceType = PieceTypes::Queen;
  chessTable_[7][3]._pieceType = PieceTypes::Queen;
  chessTable_[0][0]._pieceType = PieceTypes::Rook;
  chessTable_[0][7]._pieceType = PieceTypes::Rook;
  chessTable_[7][0]._pieceType = PieceTypes::Rook;
  chessTable_[7][7]._pieceType = PieceTypes::Rook;
  chessTable_[0][1]._pieceType = PieceTypes::Knight;
  chessTable_[0][6]._pieceType = PieceTypes::Knight;
  chessTable_[7][1]._pieceType = PieceTypes::Knight;
  chessTable_[7][6]._pieceType = PieceTypes::Knight;
  chessTable_[0][2]._pieceType = PieceTypes::Bishup;
  chessTable_[0][5]._pieceType = PieceTypes::Bishup;
  chessTable_[7][2]._pieceType = PieceTypes::Bishup;
  chessTable_[7][5]._pieceType = PieceTypes::Bishup;

  for (int i = 0; i < N_; i++) {
    chessTable_[1][i]._pieceType = PieceTypes::Pawn;
    chessTable_[6][i]._pieceType = PieceTypes::Pawn;
  }
  for (int i = 2; i < N_ - 2; i++) {
    for (int j = 0; j < N_; j++) {
      chessTable_[i][j]._pieceType = PieceTypes::VoidType;
      chessTable_[i][j]._pieceColor = PieceColor::VoidColor;
    }
  }
  for (int i = 0; i < N_; i++) {
    for (int j = 0; j < N_; j++) {
      // Setting the fields' colors
      bool iModBy2 = i % 2 == 0;
      bool jModBy2 = j % 2 == 0;
      if (iModBy2 && jModBy2)
        chessTable_[i][j]._fieldColor = FieldColor::White;

      else if (iModBy2 && !jModBy2)
        chessTable_[i][j]._fieldColor = FieldColor::Black;

      else if (!iModBy2 && jModBy2)
        chessTable_[i][j]._fieldColor = FieldColor::Black;

      else if (!iModBy2 && !jModBy2)
        chessTable_[i][j]._fieldColor = FieldColor::White;

      // Setting the pieces' colors
      if (i == 0 || i == 1)
        chessTable_[i][j]._pieceColor = PieceColor::Black;

      else if (i == N_ - 2 || i == N_ - 1)
        chessTable_[i][j]._pieceColor = PieceColor::White;
    }
  }
}

QList<QPair<int, int>> ChessModel::possibleSteps(int x, int y,
                                                 bool includeDefendedPieces,
                                                 bool attack, bool newTable) {

  PieceTypes type;
  PieceColor color;
  if (newTable) {
    type = newTable_[x][y]._pieceType;
    color = newTable_[x][y]._pieceColor;
  } else {
    type = chessTable_[x][y]._pieceType;
    color = chessTable_[x][y]._pieceColor;
  }

  QList<QPair<int, int>> fields;

  switch (type) {
  case PieceTypes::VoidType:
    return fields;
  case PieceTypes::King:
    return possibleStepsForKing(x, y, color, includeDefendedPieces, attack,
                                newTable);
  case PieceTypes::Queen:
    return possibleStepsForQueen(x, y, color, includeDefendedPieces, attack,
                                 newTable);
  case PieceTypes::Rook:
    return possibleStepsForRook(x, y, color, includeDefendedPieces, attack,
                                newTable);
  case PieceTypes::Bishup:
    return possibleStepsForBishup(x, y, color, includeDefendedPieces, attack,
                                  newTable);
  case PieceTypes::Knight:
    return possibleStepsForKnight(x, y, color, includeDefendedPieces, attack,
                                  newTable);
  case PieceTypes::Pawn:
    return possibleStepsForPawn(x, y, color, includeDefendedPieces, attack,
                                newTable);
  default:
    return fields;
  }
}

void ChessModel::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  if (std::abs(from_x / N_) >= 1 || std::abs(to_x / N_) >= 1 ||
      std::abs(from_y / N_) >= 1 || std::abs(to_y / N_) >= 1)
    return;

  else if (chessTable_[from_x][from_y]._pieceType == PieceTypes::VoidType)
    return;

  else if (chessTable_[from_x][from_y]._pieceColor ==
           chessTable_[to_x][to_y]._pieceColor)
    return;

  else if (currentPlayer_ !=
           static_cast<int>(chessTable_[from_x][from_y]._pieceColor))
    return;

  if (chessTable_[to_x][to_y]._pieceType == PieceTypes::King)
    return;

  PieceColor pc = chessTable_[to_x][to_y]._pieceColor;
  PieceTypes from_pt = chessTable_[from_x][from_y]._pieceType;

  bool enPassant =
      from_pt == PieceTypes::Pawn && chessTable_[to_x][to_y].enPassant;

  if (enPassant) {
    auto x = (to_x == 2) ? 3 : 4;
    chessTable_[x][to_y]._pieceType = PieceTypes::VoidType;
    chessTable_[x][to_y]._pieceColor = PieceColor::VoidColor;
  }

  for (int i = 0; i < N_; i++)
    for (int j = 0; j < N_; j++)
      chessTable_[i][j].enPassant = false;

  if (from_pt == PieceTypes::Pawn) {
    if (from_x == 1 && to_x == 3)
      chessTable_[2][to_y].enPassant = true;
    else if (from_x == 6 && to_x == 4)
      chessTable_[5][to_y].enPassant = true;
  }

  chessTable_[from_x][from_y].hasMoved = true;
  chessTable_[to_x][to_y].hasMoved = true;

  if (from_pt == PieceTypes::King && chessTable_[to_x][to_y].isCastlingField) {
    int rook_y = (to_y == 2) ? 3 : 5;
    int rook_x = to_x;
    if (to_y == 2) {
      chessTable_[rook_x][rook_y]._pieceType = PieceTypes::Rook;
      chessTable_[rook_x][rook_y]._pieceColor =
          chessTable_[to_x][0]._pieceColor;
      chessTable_[rook_x][rook_y].hasMoved = true;
      chessTable_[to_x][0].hasMoved = true;
      chessTable_[to_x][0]._pieceType = PieceTypes::VoidType;
      chessTable_[to_x][0]._pieceColor = PieceColor::VoidColor;
    } else {

      chessTable_[rook_x][rook_y]._pieceType = PieceTypes::Rook;
      chessTable_[rook_x][rook_y]._pieceColor =
          chessTable_[to_x][7]._pieceColor;
      chessTable_[rook_x][rook_y].hasMoved = true;
      chessTable_[to_x][7].hasMoved = true;
      chessTable_[to_x][7]._pieceType = PieceTypes::VoidType;
      chessTable_[to_x][7]._pieceColor = PieceColor::VoidColor;
    }
  }

  for (int i = 0; i < N_; i++)
    for (int j = 0; j < N_; j++)
      chessTable_[i][j].isCastlingField = false;

  chessTable_[to_x][to_y]._pieceColor = chessTable_[from_x][from_y]._pieceColor;
  chessTable_[to_x][to_y]._pieceType = from_pt;
  chessTable_[from_x][from_y]._pieceColor = PieceColor::VoidColor;
  chessTable_[from_x][from_y]._pieceType = PieceTypes::VoidType;

  if (checkGameOver()) {
    emit gameOver(currentPlayer_);
    return;
  }

  if (from_pt == PieceTypes::Pawn && (to_x == 0 || to_x == N_ - 1))
    emit pawnHasReachedEnemysBase(to_x, to_y);

  currentPlayer_ = currentPlayer_ % 2 + 1;
}

void ChessModel::switchToQueen(int x, int y, PieceTypes switchTo) {
  if (chessTable_[x][y]._pieceType != PieceTypes::Pawn)
    return;

  chessTable_[x][y]._pieceType = switchTo;
}

bool ChessModel::checkGameOver() {
  // _currentplayer's currently attacked fields
  QMap<int, QPair<int, int>> fields;
  int numberOfChecks = 0;
  QPair<int, int> lastCheck;
  QList<QPair<int, int>> lastCheckersPath;

  for (int i = 0; i < N_; i++) {
    for (int j = 0; j < N_; j++) {
      if (currentPlayer_ != static_cast<int>(chessTable_[i][j]._pieceColor))
        continue;

      QList<QPair<int, int>> possibleFields = possibleSteps(i, j, true, true);

      bool check = false;
      for (auto field : possibleFields) {
        fields.insert(field.first * N_ + field.second, field);
        if (chessTable_[field.first][field.second]._pieceType ==
                PieceTypes::King &&
            currentPlayer_ !=
                static_cast<int>(
                    chessTable_[field.first][field.second]._pieceColor)) {
          lastCheck = QPair<int, int>(i, j);
          numberOfChecks++;
          check = true;
        }
      }
      if (check) {
        lastCheckersPath.clear();
        lastCheckersPath.append(possibleFields);
      }
    }
  }

  // nextPlayer's king's position
  int king_x, king_y;
  for (int i = 0; i < N_; i++) {
    for (int j = 0; j < N_; j++) {
      if (currentPlayer_ == static_cast<int>(chessTable_[i][j]._pieceColor))
        continue;

      if (chessTable_[i][j]._pieceType == PieceTypes::King) {
        king_x = i;
        king_y = j;

        // Break
        i = N_;
        j = N_;
      }
    }
  }
  // Check for checkmate
  if (fields.contains(king_x * N_ + king_y)) {

    emit check();
    isChecked = true;

    currentPlayer_ = currentPlayer_ % 2 + 1;
    auto kingsteps = possibleStepsForKing(
        king_x, king_y, chessTable_[king_x][king_y]._pieceColor, false, true,
        false);
    currentPlayer_ = currentPlayer_ % 2 + 1;

    if (!kingsteps.isEmpty())
      return false;

    if (numberOfChecks > 1)
      return true;

    int nextPlayer = currentPlayer_ % 2 + 1;

    QSet<QPair<int, int>> nextPlayersSteps;
    for (int i = 0; i < N_; i++) {
      for (int j = 0; j < N_; j++) {
        if (nextPlayer != static_cast<int>(chessTable_[i][j]._pieceColor) ||
            chessTable_[i][j]._pieceType == PieceTypes::King)
          continue;

        auto steps = possibleSteps(i, j, false, true);
        for (auto step : steps) {
          nextPlayersSteps.insert(step);
        }
      }
    }

    // can the checker be eliminated
    if (nextPlayersSteps.contains(lastCheck))
      return false;
    // can the checker's path be blocked?
    auto checkerPiece =
        chessTable_[lastCheck.first][lastCheck.second]._pieceType;
    if (checkerPiece == PieceTypes::Pawn || checkerPiece == PieceTypes::Knight)
      return true;

    if (checkerPiece == PieceTypes::Rook || checkerPiece == PieceTypes::Queen) {
      // rook is in the same row
      if (lastCheck.first == king_x) {
        int j_min = (lastCheck.second < king_y) ? lastCheck.second : king_y;
        int j_max = (lastCheck.second < king_y) ? king_y : lastCheck.second;

        for (int j = j_min + 1; j < j_max; j++) {
          if (nextPlayersSteps.contains(QPair<int, int>(king_x, j)))
            return false;
        }
      } else if (lastCheck.second == king_y) {
        // rook is in the same column
        int i_min = (lastCheck.first < king_x) ? lastCheck.first : king_x;
        int i_max = (lastCheck.first < king_x) ? king_x : lastCheck.first;

        for (int i = i_min + 1; i < i_max; i++) {
          if (nextPlayersSteps.contains(QPair<int, int>(i, king_y)))
            return false;
        }
      }
    }
    if (checkerPiece == PieceTypes::Bishup ||
        checkerPiece == PieceTypes::Queen) {
      // Bottom_right <-> Upper_left
      if (lastCheck.first - king_x == lastCheck.second - king_y) {

        int i_min = (lastCheck.first < king_x) ? lastCheck.first : king_x;
        int i_max = (lastCheck.first < king_x) ? king_x : lastCheck.first;
        int j_min = (lastCheck.second < king_y) ? lastCheck.second : king_y;
        int j_max = (lastCheck.second < king_y) ? king_y : lastCheck.second;

        int i = i_min + 1;
        int j = j_min + 1;

        while (i < i_max && j < j_max) {
          if (nextPlayersSteps.contains(QPair<int, int>(i, j)))
            return false;

          i++;
          j++;
        }

      } else {
        // Bottom_left <-> Upper_Right
        int i_min = (lastCheck.first < king_x) ? lastCheck.first : king_x;
        int i_max = (lastCheck.first < king_x) ? king_x : lastCheck.first;
        int j_min = (lastCheck.second < king_y) ? lastCheck.second : king_y;
        int j_max = (lastCheck.second < king_y) ? king_y : lastCheck.second;

        int i = i_max - 1;
        int j = j_min + 1;

        while (i > i_min && j < j_max) {
          if (nextPlayersSteps.contains(QPair<int, int>(i, j)))
            return false;

          i--;
          j++;
        }
      }
    }

    return true;

  } // check for draw
  else {
    QList<QPair<int, int>> nextPlayersFields;
    int nextPlayer = currentPlayer_ % 2 + 1;

    for (int i = 0; i < N_; i++) {
      for (int j = 0; j < N_; j++) {
        if (nextPlayer != static_cast<int>(chessTable_[i][j]._pieceColor))
          continue;

        auto possibleFields = possibleSteps(i, j);

        for (auto field : possibleFields)
          nextPlayersFields.append(field);
      }
    }
    if (nextPlayersFields.isEmpty()) {
      // its a draw, so i'll indicate it with currentPlayer_ = 0
      currentPlayer_ = 0;
      return true;
    }
    return false;
  }
}

// TODO Fix this infinite black hole
bool ChessModel::stepCausesSelfCheck(int from_x, int from_y, int to_x, int to_y,
                                     bool attack) {

  if (!attack)
    return false;

  //_chessTable if step proceeds
  for (int i = 0; i < N_; i++) {
    for (int j = 0; j < N_; j++) {
      newTable_[i][j]._fieldColor = chessTable_[i][j]._fieldColor;
      newTable_[i][j]._pieceColor = chessTable_[i][j]._pieceColor;
      newTable_[i][j]._pieceType = chessTable_[i][j]._pieceType;
    }
  }

  newTable_[to_x][to_y]._pieceColor = newTable_[from_x][from_y]._pieceColor;
  newTable_[to_x][to_y]._pieceType = newTable_[from_x][from_y]._pieceType;
  newTable_[from_x][from_y]._pieceColor = PieceColor::VoidColor;
  newTable_[from_x][from_y]._pieceType = PieceTypes::VoidType;

  for (int i = 0; i < N_; i++) {
    for (int j = 0; j < N_; j++) {
      if (currentPlayer_ == static_cast<int>(newTable_[i][j]._pieceColor))
        continue;

      QList<QPair<int, int>> possibleFields =
          possibleSteps(i, j, false, false, true);

      for (auto field : possibleFields)
        if (newTable_[field.first][field.second]._pieceType ==
            PieceTypes::King) {
          possibleSteps(i, j, false, false, true);
          return true;
        }
    }
  }

  return false;
}

bool ChessModel::isSamePieceColor(int x, int y, PieceColor color, bool newTable,
                                  bool includeDefendedPieces) {
  if (newTable)
    return newTable_[x][y]._pieceColor == color && !includeDefendedPieces;
  else
    return chessTable_[x][y]._pieceColor == color && !includeDefendedPieces;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForKing(int x, int y, PieceColor color,
                                 bool includeDefendedPieces, bool attack,
                                 bool newTable) {
  int i_min = (x - 1 >= 0) ? x - 1 : x;
  int i_max = (x + 1 < N_) ? x + 1 : x;
  int j_min = (y - 1 >= 0) ? y - 1 : y;
  int j_max = (y + 1 < N_) ? y + 1 : y;

  QList<QPair<int, int>> fields;

  for (int i = i_min; i <= i_max; i++) {
    for (int j = j_min; j <= j_max; j++) {
      if ((i == x && j == y) ||
          (!newTable && (chessTable_[i][j]._pieceColor == color &&
                         !includeDefendedPieces)) ||
          (newTable &&
           (newTable_[i][j]._pieceColor == color && !includeDefendedPieces)))
        continue;

      if (!stepCausesSelfCheck(x, y, i, j, attack))
        fields.append(QPair<int, int>(i, j));
    }
  }
  if (chessTable_[x][y].hasMoved)
    return fields;

  if (color == PieceColor::White) {
    if (x == 7 && y == 4) {
      // right
      if (!chessTable_[7][7].hasMoved &&
          chessTable_[7][5]._pieceType == PieceTypes::VoidType &&
          chessTable_[7][6]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, 7, 6, attack)) {
        fields.append(QPair<int, int>(7, 6));
        chessTable_[7][6].isCastlingField = true;
      } else
        chessTable_[7][6].isCastlingField = false;
      // left
      if (!chessTable_[7][0].hasMoved &&
          chessTable_[7][3]._pieceType == PieceTypes::VoidType &&
          chessTable_[7][2]._pieceType == PieceTypes::VoidType &&
          chessTable_[7][1]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, 7, 2, attack)) {
        fields.append(QPair<int, int>(7, 2));
        chessTable_[7][2].isCastlingField = true;
      } else
        chessTable_[7][2].isCastlingField = false;
    }
  } else {
    if (x == 0 && y == 4) {
      // right
      if (!chessTable_[0][7].hasMoved &&
          chessTable_[0][5]._pieceType == PieceTypes::VoidType &&
          chessTable_[0][6]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, 0, 6, attack)) {
        fields.append(QPair<int, int>(0, 6));
        chessTable_[0][6].isCastlingField = true;
      } else
        chessTable_[0][6].isCastlingField = false;
      // left
      if (!chessTable_[0][0].hasMoved &&
          chessTable_[0][3]._pieceType == PieceTypes::VoidType &&
          chessTable_[0][2]._pieceType == PieceTypes::VoidType &&
          chessTable_[0][1]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, 0, 2, attack)) {
        fields.append(QPair<int, int>(0, 2));
        chessTable_[0][2].isCastlingField = true;
      } else
        chessTable_[0][2].isCastlingField = false;
    }
  }

  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForQueen(int x, int y, PieceColor color,
                                  bool includeDefendedPieces, bool attack,
                                  bool newTable) {
  QList<QPair<int, int>> fields;
  fields.append(possibleStepsForRook(x, y, color, includeDefendedPieces, attack,
                                     newTable));

  auto newFields = possibleStepsForBishup(x, y, color, includeDefendedPieces,
                                          attack, newTable);

  for (auto newfield : newFields) {
    bool duplicate = false;
    for (auto field : fields) {
      if (newfield == field)
        duplicate = true;
    }
    if (!duplicate)
      fields.append(newfield);
  }
  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForRook(int x, int y, PieceColor color,
                                 bool includeDefendedPieces, bool attack,
                                 bool newTable) {
  QList<QPair<int, int>> fields;

  // UP
  for (int i = x - 1; i >= 0; i--) {
    if (isSamePieceColor(i, y, color, newTable, includeDefendedPieces))
      break;

    if (!stepCausesSelfCheck(x, y, i, y, attack)) {
      fields.append(QPair<int, int>(i, y));
      if (!newTable && static_cast<int>(chessTable_[i][y]._pieceColor) ==
                           static_cast<int>(color) % 2 + 1)
        break;

      else if (newTable && static_cast<int>(newTable_[i][y]._pieceColor) ==
                               static_cast<int>(color) % 2 + 1)
        break;
    }

    if (isSamePieceColor(i, y, color, newTable, false))
      break;
  }

  // DOWN
  for (int i = x + 1; i < N_; i++) {
    if (isSamePieceColor(i, y, color, newTable, includeDefendedPieces))
      break;

    if (!stepCausesSelfCheck(x, y, i, y, attack)) {
      fields.append(QPair<int, int>(i, y));
      if (!newTable && static_cast<int>(chessTable_[i][y]._pieceColor) ==
                           static_cast<int>(color) % 2 + 1)
        break;

      else if (newTable && static_cast<int>(newTable_[i][y]._pieceColor) ==
                               static_cast<int>(color) % 2 + 1)
        break;
    }

    if (isSamePieceColor(i, y, color, newTable, false))
      break;
  }

  // LEFT
  for (int j = y - 1; j >= 0; j--) {
    if (isSamePieceColor(x, j, color, newTable, includeDefendedPieces))
      break;

    if (!stepCausesSelfCheck(x, y, x, j, attack)) {
      fields.append(QPair<int, int>(x, j));
      if (!newTable && static_cast<int>(chessTable_[x][j]._pieceColor) ==
                           static_cast<int>(color) % 2 + 1)
        break;

      else if (newTable && static_cast<int>(newTable_[x][j]._pieceColor) ==
                               static_cast<int>(color) % 2 + 1)
        break;
    }

    if (isSamePieceColor(x, j, color, newTable, false))
      break;
  }

  // RIGHT
  for (int j = y + 1; j < N_; j++) {
    if (isSamePieceColor(x, j, color, newTable, includeDefendedPieces))
      break;

    if (!stepCausesSelfCheck(x, y, x, j, attack)) {
      fields.append(QPair<int, int>(x, j));
      if (!newTable && static_cast<int>(chessTable_[x][j]._pieceColor) ==
                           static_cast<int>(color) % 2 + 1)
        break;

      else if (newTable && static_cast<int>(newTable_[x][j]._pieceColor) ==
                               static_cast<int>(color) % 2 + 1)
        break;
    }

    if (isSamePieceColor(x, j, color, newTable, false))
      break;
  }

  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForBishup(int x, int y, PieceColor color,
                                   bool includeDefendedPieces, bool attack,
                                   bool newTable) {
  QList<QPair<int, int>> *fields = new QList<QPair<int, int>>();

  // returns true if it needs to stop looking in that direction
  auto processLambda = [=](int i, int j) -> bool {
    if (isSamePieceColor(i, j, color, newTable, includeDefendedPieces))
      return true;

    if (!stepCausesSelfCheck(x, y, i, j, attack)) {
      fields->append(QPair<int, int>(i, j));

      if (!newTable && static_cast<int>(chessTable_[i][j]._pieceColor) ==
                           static_cast<int>(color) % 2 + 1)
        return true;

      else if (newTable && static_cast<int>(newTable_[i][j]._pieceColor) ==
                               static_cast<int>(color) % 2 + 1)
        return true;
    } else
      return true;

    if (isSamePieceColor(i, j, color, newTable, false))
      return true;

    return false;
  };
  // LEFTUP

  int i = x - 1;
  for (int j = y - 1; j >= 0 && i >= 0; j-- && i--) {
    if (processLambda(i, j))
      break;
  }

  // LEFTDOWN

  i = x + 1;
  for (int j = y - 1; j >= 0 && i < N_; j-- && i++) {
    if (processLambda(i, j))
      break;
  }

  // RIGHTUP

  i = x - 1;
  for (int j = y + 1; j < N_ && i >= 0; j++ && i--) {
    if (processLambda(i, j))
      break;
  }

  // RIGHTDOWN

  i = x + 1;
  for (int j = y + 1; j < N_ && i < N_; j++ && i++) {
    if (processLambda(i, j))
      break;
  }

  auto results = *fields;
  delete fields;
  return results;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForKnight(int x, int y, PieceColor color,
                                   bool includeDefendedPieces, bool attack,
                                   bool newTable) {
  int i_min = (x - 2 >= 0) ? (x - 2) : ((x - 1 >= 0) ? x - 1 : 0);
  int i_max = (x + 2 < N_) ? (x + 2) : ((x + 1 < N_) ? x + 1 : N_ - 1);
  int j_min = (y - 2 >= 0) ? (y - 2) : ((y - 1 >= 0) ? y - 1 : 0);
  int j_max = (y + 2 < N_) ? (y + 2) : ((y + 1 < N_) ? y + 1 : N_ - 1);

  QList<QPair<int, int>> fields;

  for (int i = i_min; i <= i_max; i++) {
    for (int j = j_min; j <= j_max; j++) {
      if ((i == x && j == y) ||
          isSamePieceColor(i, j, color, newTable, includeDefendedPieces))
        continue;

      int x_diff = std::abs(i - x);
      int y_diff = std::abs(j - y);

      if (x_diff == 1 && y_diff == 2 &&
          !stepCausesSelfCheck(x, y, i, j, attack))
        fields.append(QPair<int, int>(i, j));

      else if (x_diff == 2 && y_diff == 1 &&
               !stepCausesSelfCheck(x, y, i, j, attack))
        fields.append(QPair<int, int>(i, j));
    }
  }

  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForPawn(int x, int y, PieceColor color,
                                 bool includeDefendedPieces, bool attack,
                                 bool newTable) {
  QList<QPair<int, int>> fields;
  if (color == PieceColor::White && x > 0) {
    bool startPosition = x == 6;
    if (newTable) {
      if (attack && startPosition &&
          newTable_[x - 2][y]._pieceType == PieceTypes::VoidType &&
          newTable_[x - 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x - 2, y, attack))
        fields.append(QPair<int, int>(x - 2, y));

      if (attack && newTable_[x - 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x - 1, y, attack))
        fields.append(QPair<int, int>(x - 1, y));

      if ((y > 0 && (newTable_[x - 1][y - 1]._pieceColor == PieceColor::Black ||
                     (newTable_[x - 1][y - 1]._pieceColor == color &&
                      includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y - 1, attack))
        fields.append(QPair<int, int>(x - 1, y - 1));

      if ((y < N_ - 1 &&
           (newTable_[x - 1][y + 1]._pieceColor == PieceColor::Black ||
            (newTable_[x - 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y + 1, attack))
        fields.append(QPair<int, int>(x - 1, y + 1));
      // enpassant left and right
      if (y > 0 && x == 3 && newTable_[2][y - 1].enPassant &&
          !stepCausesSelfCheck(x, y, 2, y - 1, attack))
        fields.append(QPair<int, int>(2, y - 1));

      if (y < (N_ - 1) && x == 3 && newTable_[2][y + 1].enPassant &&
          !stepCausesSelfCheck(x, y, 2, y + 1, attack))
        fields.append(QPair<int, int>(2, y + 1));
    } else {
      if (attack && startPosition &&
          chessTable_[x - 2][y]._pieceType == PieceTypes::VoidType &&
          chessTable_[x - 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x - 2, y, attack))
        fields.append(QPair<int, int>(x - 2, y));

      if (attack && chessTable_[x - 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x - 1, y, attack))
        fields.append(QPair<int, int>(x - 1, y));

      if ((y > 0 &&
           (chessTable_[x - 1][y - 1]._pieceColor == PieceColor::Black ||
            (chessTable_[x - 1][y - 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y - 1, attack))
        fields.append(QPair<int, int>(x - 1, y - 1));

      if ((y < N_ - 1 &&
           (chessTable_[x - 1][y + 1]._pieceColor == PieceColor::Black ||
            (chessTable_[x - 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y + 1, attack))
        fields.append(QPair<int, int>(x - 1, y + 1));
      // enpassant left and right
      if (y > 0 && x == 3 && chessTable_[2][y - 1].enPassant &&
          !stepCausesSelfCheck(x, y, 2, y - 1, attack))
        fields.append(QPair<int, int>(2, y - 1));

      if (y < (N_ - 1) && x == 3 && chessTable_[2][y + 1].enPassant &&
          !stepCausesSelfCheck(x, y, 2, y + 1, attack))
        fields.append(QPair<int, int>(2, y + 1));
    }
  }

  if (color == PieceColor::Black && x < N_ - 1) {
    bool startPosition = x == 1;
    if (newTable) {
      if (attack && startPosition &&
          newTable_[x + 2][y]._pieceType == PieceTypes::VoidType &&
          newTable_[x + 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x + 2, y, attack))
        fields.append(QPair<int, int>(x + 2, y));

      if (attack && newTable_[x + 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x + 1, y, attack))
        fields.append(QPair<int, int>(x + 1, y));

      if ((y > 0 && (newTable_[x + 1][y - 1]._pieceColor == PieceColor::White ||
                     (newTable_[x + 1][y - 1]._pieceColor == color &&
                      includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y - 1, attack))
        fields.append(QPair<int, int>(x + 1, y - 1));

      if (((y < N_ - 1 &&
                newTable_[x + 1][y + 1]._pieceColor == PieceColor::White ||
            (newTable_[x + 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y + 1, attack))
        fields.append(QPair<int, int>(x + 1, y + 1));
      // enpassant left and right
      if (y > 0 && x == 4 && newTable_[5][y - 1].enPassant &&
          !stepCausesSelfCheck(x, y, 5, y - 1, attack))
        fields.append(QPair<int, int>(5, y - 1));

      if (y < (N_ - 1) && x == 4 && newTable_[5][y + 1].enPassant &&
          !stepCausesSelfCheck(x, y, 5, y + 1, attack))
        fields.append(QPair<int, int>(5, y + 1));
    } else {
      if (attack && startPosition &&
          chessTable_[x + 2][y]._pieceType == PieceTypes::VoidType &&
          chessTable_[x + 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x + 2, y, attack))
        fields.append(QPair<int, int>(x + 2, y));

      if (attack && chessTable_[x + 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x + 1, y, attack))
        fields.append(QPair<int, int>(x + 1, y));

      if ((y > 0 &&
           (chessTable_[x + 1][y - 1]._pieceColor == PieceColor::White ||
            (chessTable_[x + 1][y - 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y - 1, attack))
        fields.append(QPair<int, int>(x + 1, y - 1));

      if (((y < N_ - 1 &&
                chessTable_[x + 1][y + 1]._pieceColor == PieceColor::White ||
            (chessTable_[x + 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y + 1, attack))
        fields.append(QPair<int, int>(x + 1, y + 1));
      // enpassant left and right
      if (y > 0 && x == 4 && chessTable_[5][y - 1].enPassant &&
          !stepCausesSelfCheck(x, y, 5, y - 1, attack))
        fields.append(QPair<int, int>(5, y - 1));

      if (y < (N_ - 1) && x == 4 && chessTable_[5][y + 1].enPassant &&
          !stepCausesSelfCheck(x, y, 5, y + 1, attack))
        fields.append(QPair<int, int>(5, y + 1));
    }
  }
  return fields;
}

ChessField ChessModel::getField(int x, int y) { return chessTable_[x][y]; }

void ChessModel::setHighlighted(int x, int y, bool highlighted) {
  chessTable_[x][y].highlighted = highlighted;
}

QJsonObject ChessModel::serializeField(int x, int y) {
  return QJsonObject{
      {"FieldColor", static_cast<int>(chessTable_[x][y]._fieldColor)},
      {"PieceColor", static_cast<int>(chessTable_[x][y]._pieceColor)},
      {"PieceType", static_cast<int>(chessTable_[x][y]._pieceType)},
      {"Highlighted", chessTable_[x][y].highlighted},
      {"EnPassant", chessTable_[x][y].enPassant},
      {"HasMoved", chessTable_[x][y].hasMoved},
      {"IsCastlingField", chessTable_[x][y].isCastlingField}};
}

void ChessModel::deSerializeField(QJsonObject fieldJson, int x, int y) {
  chessTable_[x][y]._fieldColor =
      static_cast<FieldColor>(fieldJson["FieldColor"].toInt());
  chessTable_[x][y]._pieceColor =
      static_cast<PieceColor>(fieldJson["PieceColor"].toInt());
  chessTable_[x][y]._pieceType =
      static_cast<PieceTypes>(fieldJson["PieceType"].toInt());
  chessTable_[x][y].enPassant = fieldJson["EnPassant"].toBool();
  chessTable_[x][y].hasMoved = fieldJson["HasMoved"].toBool();
  chessTable_[x][y].highlighted = fieldJson["Highlighted"].toBool();
  chessTable_[x][y].isCastlingField = fieldJson["IsCastlingField"].toBool();
}

int ChessModel::getCurrentPlayer() { return currentPlayer_; }

bool ChessModel::isMyPiece(int x, int y) {
  return currentPlayer_ == static_cast<int>(chessTable_[x][y]._pieceColor);
}
