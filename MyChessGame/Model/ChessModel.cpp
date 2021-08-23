#include "ChessModel.h"

ChessModel::ChessModel() : _N(8), _currentPlayer(1) {
  _chessTable = new ChessField *[8];
  for (int i = 0; i < _N; i++)
    _chessTable[i] = new ChessField[8];

  _newTable = new ChessField *[8];
  for (int i = 0; i < _N; i++)
    _newTable[i] = new ChessField[8];

  // Setting up the table's piecetypes
  _chessTable[0][4]._pieceType = PieceTypes::King;
  _chessTable[7][4]._pieceType = PieceTypes::King;
  _chessTable[0][3]._pieceType = PieceTypes::Queen;
  _chessTable[7][3]._pieceType = PieceTypes::Queen;
  _chessTable[0][0]._pieceType = PieceTypes::Rook;
  _chessTable[0][7]._pieceType = PieceTypes::Rook;
  _chessTable[7][0]._pieceType = PieceTypes::Rook;
  _chessTable[7][7]._pieceType = PieceTypes::Rook;
  _chessTable[0][1]._pieceType = PieceTypes::Knight;
  _chessTable[0][6]._pieceType = PieceTypes::Knight;
  _chessTable[7][1]._pieceType = PieceTypes::Knight;
  _chessTable[7][6]._pieceType = PieceTypes::Knight;
  _chessTable[0][2]._pieceType = PieceTypes::Bishup;
  _chessTable[0][5]._pieceType = PieceTypes::Bishup;
  _chessTable[7][2]._pieceType = PieceTypes::Bishup;
  _chessTable[7][5]._pieceType = PieceTypes::Bishup;

  for (int i = 0; i < _N; i++) {
    _chessTable[1][i]._pieceType = PieceTypes::Pawn;
    _chessTable[6][i]._pieceType = PieceTypes::Pawn;
  }
  for (int i = 2; i < _N - 2; i++) {
    for (int j = 0; j < _N; j++) {
      _chessTable[i][j]._pieceType = PieceTypes::VoidType;
      _chessTable[i][j]._pieceColor = PieceColor::VoidColor;
    }
  }
  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      // Setting the fields' colors
      bool iModBy2 = i % 2 == 0;
      bool jModBy2 = j % 2 == 0;
      if (iModBy2 && jModBy2)
        _chessTable[i][j]._fieldColor = FieldColor::White;

      else if (iModBy2 && !jModBy2)
        _chessTable[i][j]._fieldColor = FieldColor::Black;

      else if (!iModBy2 && jModBy2)
        _chessTable[i][j]._fieldColor = FieldColor::Black;

      else if (!iModBy2 && !jModBy2)
        _chessTable[i][j]._fieldColor = FieldColor::White;

      // Setting the pieces' colors
      if (i == 0 || i == 1)
        _chessTable[i][j]._pieceColor = PieceColor::Black;

      else if (i == _N - 2 || i == _N - 1)
        _chessTable[i][j]._pieceColor = PieceColor::White;
    }
  }
}

QList<QPair<int, int>> ChessModel::possibleSteps(int x, int y,
                                                 bool includeDefendedPieces,
                                                 bool attack, bool newTable) {
  PieceTypes type = _chessTable[x][y]._pieceType;
  PieceColor color = _chessTable[x][y]._pieceColor;

  QList<QPair<int, int>> fields;

  switch (type) {
  case PieceTypes::VoidType:
    return fields;
  case PieceTypes::King:
    return possibleStepsForKing(x, y, color, includeDefendedPieces, newTable);
  case PieceTypes::Queen:
    return possibleStepsForQueen(x, y, color, includeDefendedPieces, newTable);
  case PieceTypes::Rook:
    return possibleStepsForRook(x, y, color, includeDefendedPieces, newTable);
  case PieceTypes::Bishup:
    return possibleStepsForBishup(x, y, color, includeDefendedPieces, newTable);
  case PieceTypes::Knight:
    return possibleStepsForKnight(x, y, color, includeDefendedPieces, newTable);
  case PieceTypes::Pawn:
    return possibleStepsForPawn(x, y, color, includeDefendedPieces, attack,
                                newTable);
  default:
    return fields;
  }
}

void ChessModel::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  if (std::abs(from_x / _N) >= 1 || std::abs(to_x / _N) >= 1 ||
      std::abs(from_y / _N) >= 1 || std::abs(to_y / _N) >= 1)
    return;

  else if (_chessTable[from_x][from_y]._pieceType == PieceTypes::VoidType)
    return;

  else if (_chessTable[from_x][from_y]._pieceColor ==
           _chessTable[to_x][to_y]._pieceColor)
    return;

  else if (_currentPlayer !=
           static_cast<int>(_chessTable[from_x][from_y]._pieceColor))
    return;

  PieceColor pc = _chessTable[to_x][to_y]._pieceColor;

  _chessTable[to_x][to_y]._pieceColor = _chessTable[from_x][from_y]._pieceColor;
  _chessTable[to_x][to_y]._pieceType = _chessTable[from_x][from_y]._pieceType;
  _chessTable[from_x][from_y]._pieceColor = PieceColor::VoidColor;
  _chessTable[from_x][from_y]._pieceType = PieceTypes::VoidType;

  if (pc != PieceColor::VoidColor)
    emit stepped(true);
  else
    emit stepped(false);

  if (checkGameOver())
    emit gameOver(_currentPlayer);

  _currentPlayer = _currentPlayer % 2 + 1;
}

bool ChessModel::checkGameOver() {
  // _currentplayer's currently attacked fields
  QMap<int, QPair<int, int>> fields;
  int numberOfChecks = 0;
  QPair<int, int> lastCheck;

  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if (_currentPlayer != static_cast<int>(_chessTable[i][j]._pieceColor))
        continue;

      QList<QPair<int, int>> possibleFields = possibleSteps(i, j, false, true);

      for (auto field : possibleFields) {
        if (_chessTable[field.first][field.second]._pieceColor !=
            PieceColor::VoidColor) {
          fields.insert(field.first * _N + field.second, field);
          if (_chessTable[field.first][field.second]._pieceType ==
              PieceTypes::King) {
            lastCheck = QPair<int, int>(i, j);
            numberOfChecks++;
          }
        }
      }
    }
  }

  // nextPlayer's king's position
  int king_x, king_y;
  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if (_currentPlayer == static_cast<int>(_chessTable[i][j]._pieceColor))
        continue;

      if (_chessTable[i][j]._pieceType == PieceTypes::King) {
        king_x = i;
        king_y = j;

        // Break
        i = _N;
        j = _N;
      }
    }
  }
  // Check for checkmate
  if (fields.contains(king_x * _N + king_y)) {

    emit check();

    int i_min = (king_x - 1 >= 0) ? king_x : 0;
    int i_max = (king_x + 1 < _N) ? king_x : _N - 1;
    int j_min = (king_y - 1 >= 0) ? king_y : 0;
    int j_max = (king_y + 1 < _N) ? king_y : _N - 1;

    bool kingCanMove = false;
    for (int i = i_min; i < i_max; i++) {
      for (int j = j_min; j < j_max; j++) {
        if (!fields.contains(i * _N + j)) {
          kingCanMove = true;
          // break
          i = _N;
          j = _N;
        }
      }
    }
    if (kingCanMove)
      return false;

    if (numberOfChecks > 1)
      return true;

    auto attacker = getSingleChecker();

    int nextPlayer = _currentPlayer % 2 + 1;
    for (int i = 0; i < _N; i++) {
      for (int j = 0; j < _N; j++) {
        if (nextPlayer != static_cast<int>(_chessTable[i][j]._pieceColor))
          continue;

        if (isValidStep(i, j, attacker.first, attacker.second))
          return false;
      }
    }
    return true;

  } // check for draw
  else {
    QList<QPair<int, int>> nextPlayersFields;
    int nextPlayer = _currentPlayer % 2 + 1;

    for (int i = 0; i < _N; i++) {
      for (int j = 0; j < _N; j++) {
        if (nextPlayer != static_cast<int>(_chessTable[i][j]._pieceColor))
          continue;

        auto possibleFields = possibleSteps(i, j);

        for (auto field : possibleFields)
          nextPlayersFields.append(field);
      }
    }
    if (nextPlayersFields.isEmpty()) {
      // its a draw, so i'll indicate it with _currentPlayer = 0
      _currentPlayer = 0;
      return true;
    }
    return false;
  }
}
bool ChessModel::stepCausesSelfCheck(int from_x, int from_y, int to_x,
                                     int to_y) {
  // _currentplayer's currently attacked fields
  QMap<int, QPair<int, int>> fields;
  //_chessTable if step proceeds
  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      _newTable[i][j]._fieldColor = _chessTable[i][j]._fieldColor;
      _newTable[i][j]._pieceColor = _chessTable[i][j]._pieceColor;
      _newTable[i][j]._pieceType = _chessTable[i][j]._pieceType;
    }
  }

  _newTable[to_x][to_y]._pieceColor = _newTable[from_x][from_y]._pieceColor;
  _newTable[to_x][to_y]._pieceType = _newTable[from_x][from_y]._pieceType;
  _newTable[from_x][from_y]._pieceColor = PieceColor::VoidColor;
  _newTable[from_x][from_y]._pieceType = PieceTypes::VoidType;

  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if (_currentPlayer == static_cast<int>(_newTable[i][j]._pieceColor))
        continue;

      QList<QPair<int, int>> possibleFields =
          possibleSteps(i, j, false, false, true);

      for (auto field : possibleFields) {
        if (_newTable[field.first][field.second]._pieceColor !=
            PieceColor::VoidColor) {
          fields.insert(field.first * _N + field.second, field);
        }
      }
    }
  }

  // nextPlayer's king's position
  int king_x, king_y;
  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if (_currentPlayer != static_cast<int>(_newTable[i][j]._pieceColor))
        continue;

      if (_newTable[i][j]._pieceType == PieceTypes::King) {
        king_x = i;
        king_y = j;

        // Break
        i = _N;
        j = _N;
      }
    }
  }

  if (fields.contains(king_x * _N + king_y))
    return true;

  return false;
}

bool ChessModel::isValidStep(int from_x, int from_y, int to_x, int to_y) {
  auto steps = possibleSteps(from_x, from_y, false, true);
  auto pair = QPair<int, int>(to_x, to_y);
  for (auto step : steps)
    if (step == pair)
      return true;

  return false;
}

QPair<int, int> ChessModel::getSingleChecker() {
  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if (_currentPlayer != static_cast<int>(_chessTable[i][j]._pieceColor))
        continue;

      QList<QPair<int, int>> possibleFields = possibleSteps(i, j, false, true);

      for (auto field : possibleFields)
        if (_chessTable[field.first][field.second]._pieceType ==
            PieceTypes::King)
          return QPair<int, int>(i, j);
    }
  }
  return QPair<int, int>(-1, -1);
}

QList<QPair<int, int>>
ChessModel::possibleStepsForKing(int x, int y, PieceColor color,
                                 bool includeDefendedPieces, bool newTable) {
  int i_min = (x - 1 >= 0) ? x : 0;
  int i_max = (x + 1 < _N) ? x : _N - 1;
  int j_min = (y - 1 >= 0) ? y : 0;
  int j_max = (y + 1 < _N) ? y : _N - 1;

  QList<QPair<int, int>> fields;

  for (int i = i_min; i < i_max; i++) {
    for (int j = j_min; j < j_max; j++) {
      if ((i == x && j == y) ||
          (!newTable && (_chessTable[i][j]._pieceColor == color &&
                         !includeDefendedPieces)) ||
          (newTable &&
           (_newTable[i][j]._pieceColor == color && !includeDefendedPieces)))
        continue;

      fields.append(QPair<int, int>(i, j));
    }
  }
  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForQueen(int x, int y, PieceColor color,
                                  bool includeDefendedPieces, bool newTable) {
  QList<QPair<int, int>> fields;

  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if ((i == x && j == y) ||
          (!newTable && _chessTable[i][j]._pieceColor == color &&
           !includeDefendedPieces) ||
          (newTable &&
           (_newTable[i][j]._pieceColor == color && !includeDefendedPieces)))
        continue;

      bool sameRow = i % _N == x % _N;
      bool sameColumn = j % _N == y % _N;
      bool sameDiagonal = i % _N - x % _N == j % _N - y % _N;
      if ((sameRow || sameColumn || sameDiagonal) &&
          !stepCausesSelfCheck(x, y, i, j))
        fields.append(QPair<int, int>(i, j));
    }
  }
  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForRook(int x, int y, PieceColor color,
                                 bool includeDefendedPieces, bool newTable) {
  QList<QPair<int, int>> fields;

  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if ((i == x && j == y) ||
          (!newTable && _chessTable[i][j]._pieceColor == color &&
           !includeDefendedPieces) ||
          (newTable &&
           (_newTable[i][j]._pieceColor == color && !includeDefendedPieces)))
        continue;

      bool sameRow = i % _N == x % _N;
      bool sameColumn = j % _N == y % _N;
      if ((sameRow || sameColumn) && !stepCausesSelfCheck(x, y, i, j))
        fields.append(QPair<int, int>(i, j));
    }
  }
  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForBishup(int x, int y, PieceColor color,
                                   bool includeDefendedPieces, bool newTable) {
  QList<QPair<int, int>> fields;

  for (int i = 0; i < _N; i++) {
    for (int j = 0; j < _N; j++) {
      if ((i == x && j == y) ||
          (!newTable && _chessTable[i][j]._pieceColor == color &&
           !includeDefendedPieces) ||
          (newTable &&
           (_newTable[i][j]._pieceColor == color && !includeDefendedPieces)))
        continue;

      bool sameDiagonal = i % _N - x % _N == j % _N - y % _N;
      if (sameDiagonal && !stepCausesSelfCheck(x, y, i, j))
        fields.append(QPair<int, int>(i, j));
    }
  }
  return fields;
}

QList<QPair<int, int>>
ChessModel::possibleStepsForKnight(int x, int y, PieceColor color,
                                   bool includeDefendedPieces, bool newTable) {
  int i_min = (x - 2 >= 0) ? x : 0;
  int i_max = (x + 2 < _N) ? x : _N - 1;
  int j_min = (y - 2 >= 0) ? y : 0;
  int j_max = (y + 2 < _N) ? y : _N - 1;

  QList<QPair<int, int>> fields;

  for (int i = i_min; i < i_max; i++) {
    for (int j = j_min; j < j_max; j++) {
      if ((i == x && j == y) ||
          (!newTable && _chessTable[i][j]._pieceColor == color &&
           !includeDefendedPieces) ||
          (newTable &&
           (_newTable[i][j]._pieceColor == color && !includeDefendedPieces)))
        continue;

      int x_mod = i % _N - x % _N;
      int y_mod = j % _N - y % _N;

      if ((std::abs(x_mod) == 1 && std::abs(y_mod) == 2) &&
          !stepCausesSelfCheck(x, y, i, j))
        fields.append(QPair<int, int>(i, j));

      else if ((std::abs(x_mod) == 2 && std::abs(y_mod) == 1) &&
               !stepCausesSelfCheck(x, y, i, j))
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
  if (color == PieceColor::White && x < 0) {
    if (newTable) {
      if (!attack && _newTable[x - 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x - 1, y))
        fields.append(QPair<int, int>(x - 1, y));

      if ((y > 0 && (_newTable[x - 1][y - 1]._pieceColor == PieceColor::Black ||
                     (_newTable[x - 1][y - 1]._pieceColor == color &&
                      includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y - 1))
        fields.append(QPair<int, int>(x - 1, y - 1));

      if ((y < _N - 1 &&
           (_newTable[x - 1][y + 1]._pieceColor == PieceColor::Black ||
            (_newTable[x - 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y + 1))
        fields.append(QPair<int, int>(x - 1, y + 1));
    } else {
      if (!attack && _chessTable[x - 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x - 1, y))
        fields.append(QPair<int, int>(x - 1, y));

      if ((y > 0 &&
           (_chessTable[x - 1][y - 1]._pieceColor == PieceColor::Black ||
            (_chessTable[x - 1][y - 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y - 1))
        fields.append(QPair<int, int>(x - 1, y - 1));

      if ((y < _N - 1 &&
           (_chessTable[x - 1][y + 1]._pieceColor == PieceColor::Black ||
            (_chessTable[x - 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x - 1, y + 1))
        fields.append(QPair<int, int>(x - 1, y + 1));
    }
  }

  if (color == PieceColor::Black && x > _N - 1) {
    if (newTable) {
      if (!attack && _newTable[x + 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x + 1, y))
        fields.append(QPair<int, int>(x + 1, y));

      if ((y > 0 && (_newTable[x + 1][y - 1]._pieceColor == PieceColor::White ||
                     (_newTable[x + 1][y - 1]._pieceColor == color &&
                      includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y - 1))
        fields.append(QPair<int, int>(x + 1, y - 1));

      if (((y < _N - 1 &&
                _newTable[x + 1][y + 1]._pieceColor == PieceColor::White ||
            (_newTable[x + 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y + 1))
        fields.append(QPair<int, int>(x + 1, y + 1));
    } else {
      if (!attack && _chessTable[x + 1][y]._pieceType == PieceTypes::VoidType &&
          !stepCausesSelfCheck(x, y, x + 1, y))
        fields.append(QPair<int, int>(x + 1, y));

      if ((y > 0 &&
           (_chessTable[x + 1][y - 1]._pieceColor == PieceColor::White ||
            (_chessTable[x + 1][y - 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y - 1))
        fields.append(QPair<int, int>(x + 1, y - 1));

      if (((y < _N - 1 &&
                _chessTable[x + 1][y + 1]._pieceColor == PieceColor::White ||
            (_chessTable[x + 1][y + 1]._pieceColor == color &&
             includeDefendedPieces))) &&
          !stepCausesSelfCheck(x, y, x + 1, y + 1))
        fields.append(QPair<int, int>(x + 1, y + 1));
    }
  }
  return fields;
}
