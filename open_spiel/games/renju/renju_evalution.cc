#include <cmath>
#include "open_spiel/games/renju/renju_evalution.h"
#include <iostream>

namespace open_spiel {
namespace renju {

constexpr int ANY = -1;
constexpr int EMPTY = 0;
constexpr int BLACK = 1;
constexpr int WHITE = 2;

int getCurrentPlayer(const std::array<std::array<int, 15>, 15>& board) {
  int black = 0;
  int white = 0;
  for (int i=0; i<15; i++) {
    for (int j=0; j<15; j++) {
      if (board[i][j] == BLACK) {
        black ++;
      }
      else if (board[i][j] == WHITE) {
        white ++;
      }
    }
  }
  if (black > white) {
    return WHITE;
  }
  return BLACK;
}

bool isMatch(int chess, int match) {
  if (match == ANY) {
    return true;
  }
  return chess == match;
}

bool isMatch(std::array<int, 6> chesses, std::array<int, 6> matches) {
  for (int i = 0; i < 6; ++i) {
    if (!isMatch(chesses[i], matches[i])) {
      return false;
    }
  }
  return true;
}

bool RenjuEvalation::getChesses(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color, std::array<int, 6>& chesses) {
  bool flag = true;
  for (int i=0; i<6; i++) {
    chesses[i] = board[x + dx * i][y + dy * i];
    if (chesses[i] == color && status[x + dx * i][y + dy * i] == UNKOWN) {
      flag = false;
    }
  }
  return flag;
}

void RenjuEvalation::evaluateFive(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color) {
  std::array<int, 6> chesses;
  if (getChesses(board, x, y, dx, dy, color, chesses)) {
    return;
  }
  int other = 3 - color;
  // 连五
  // X X X X X *
  // * X X X X X
  if (
    isMatch(chesses, {color, color, color, color, color, ANY}) ||
    isMatch(chesses, {ANY, color, color, color, color, color})
  ) {
    fillStatus(board, x, y, dx, dy, color, Five);
  }
}
void RenjuEvalation::evaluateFour(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color) {
  std::array<int, 6> chesses;
  if (getChesses(board, x, y, dx, dy, color, chesses)) {
    return;
  }
  int other = 3 - color;
  // 活四
  // . X X X X .
  if (
    isMatch(chesses, {EMPTY, color, color, color, color, EMPTY})
  ) {
    fillStatus(board, x, y, dx, dy, color, Four);
  }
}

void RenjuEvalation::evaluateDeadFour(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color) {
  std::array<int, 6> chesses;
  if (getChesses(board, x, y, dx, dy, color, chesses)) {
    return;
  }
  int other = 3 - color;
  // 冲四
  // O X X X X .
  // . X X X X O
  // X X X . X *
  // X X . X X *
  // X . X X X *
  if (
    isMatch(chesses, {other, color, color, color, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, color, color, color, other}) ||
    isMatch(chesses, {color, color, color, EMPTY, color, ANY}) ||
    isMatch(chesses, {color, color, EMPTY, color, color, ANY}) ||
    isMatch(chesses, {color, EMPTY, color, color, color, ANY})
  ) {
    fillStatus(board, x, y, dx, dy, color, DeadFour);
  }
}

void RenjuEvalation::evaluateThree(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color) {
  std::array<int, 6> chesses;
  if (getChesses(board, x, y, dx, dy, color, chesses)) {
    return;
  }
  int other = 3 - color;
  // 活三
  // . . X X X .
  // . X X X . .
  // . X . X X .
  // . X X . X .
  if (
    isMatch(chesses, {EMPTY, EMPTY, color, color, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, color, color, EMPTY, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, EMPTY, color, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, color, EMPTY, color, EMPTY})
  ) {
    fillStatus(board, x, y, dx, dy, color, Three);
  }
}

void RenjuEvalation::evaluateDeadThree(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color) {
  std::array<int, 6> chesses;
  if (getChesses(board, x, y, dx, dy, color, chesses)) {
    return;
  }
  int other = 3 - color;
  // 冲三
  // . . X X X O
  // O X X X . .
  // O X . X X .
  // . X . X X O
  // O X X . X .
  // . X X . X O
  if (
    isMatch(chesses, {EMPTY, EMPTY, color, color, color, other}) ||
    isMatch(chesses, {other, color, color, color, EMPTY, EMPTY}) ||
    isMatch(chesses, {other, color, EMPTY, color, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, EMPTY, color, color, other}) ||
    isMatch(chesses, {other, color, color, EMPTY, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, color, EMPTY, color, other})
  ) {
    fillStatus(board, x, y, dx, dy, color, DeadThree);
  }
}

void RenjuEvalation::evaluateTwo(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color) {
  std::array<int, 6> chesses;
  if (getChesses(board, x, y, dx, dy, color, chesses)) {
    return;
  }
  int other = 3 - color;
  // 活二
  // . . X X . .
  // . . X . X .
  // . . . X X .
  // . X X . . .
  // . X . X . .
  // . X . . X .
  if (
    isMatch(chesses, {EMPTY, EMPTY, color, color, EMPTY, EMPTY}) ||
    isMatch(chesses, {EMPTY, EMPTY, color, EMPTY, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, EMPTY, EMPTY, color, color, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, color, EMPTY, EMPTY, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, EMPTY, color, EMPTY, EMPTY}) ||
    isMatch(chesses, {EMPTY, color, EMPTY, EMPTY, color, EMPTY})
  ) {
    fillStatus(board, x, y, dx, dy, color, Two);
  }
}

void RenjuEvalation::fillStatus(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color, const int cellState) {
  for (int i=0; i<6; i++) {
    if (board[x + dx * i][y + dy * i] == color) {
      status[x + dx * i][y + dy * i] = cellState;
    }
  }
  if (color == BLACK) {
    blackScore[cellState] ++;
  } else {
    whiteScore[cellState] ++;
  }
}

void RenjuEvalation::evaluateLine(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy) {
  auto functions = {
    &RenjuEvalation::evaluateFive,
    &RenjuEvalation::evaluateFour,
    &RenjuEvalation::evaluateDeadFour,
    &RenjuEvalation::evaluateThree,
    &RenjuEvalation::evaluateDeadThree,
    &RenjuEvalation::evaluateTwo
  };
  for (auto& function: functions) {
    for (int i=0; i<15; i++) {
      int ix = x + dx * i;
      int iy = y + dy * i;
      int jx = ix + 5 * dx;
      int jy = iy + 5 * dy;
      if (
        ix >= 0 && ix < 15 &&
        iy >= 0 && iy < 15 &&
        jx >= 0 && jx < 15 &&
        jy >= 0 && jy < 15
      ) {
        (this->*function)(board, ix, iy, dx, dy, BLACK);
        (this->*function)(board, ix, iy, dx, dy, WHITE);
      }
    }
  }
}

double RenjuEvalation::evaluate(const std::array<std::array<int, 15>, 15>& board) {
  std::fill(blackScore.begin(), blackScore.end(), 0);
  std::fill(whiteScore.begin(), whiteScore.end(), 0);
  currentPlayer = getCurrentPlayer(board);

  // 横
  for (auto& row : status) {
    std::fill(row.begin(), row.end(), UNKOWN);
  }
  for (int i=0; i<15; i++) {
    evaluateLine(board, 0, i, 1, 0);
  }
  // 竖
  for (auto& row : status) {
    std::fill(row.begin(), row.end(), UNKOWN);
  }
  for (int i=0; i<15; i++) {
    evaluateLine(board, i, 0, 0, 1);
  }
  // 左上-右下
  for (auto& row : status) {
    std::fill(row.begin(), row.end(), UNKOWN);
  }
  for (int i=0; i<15; i++) {
    evaluateLine(board, 0, i, 1, 1);
  }
  for (int i=1; i<15; i++) {
    evaluateLine(board, i, 0, 1, 1);
  }
  // 右上-左下
  for (auto& row : status) {
    std::fill(row.begin(), row.end(), UNKOWN);
  }
  for (int i=0; i<15; i++) {
    evaluateLine(board, 0, i, 1, -1);
  }
  for (int i=1; i<15; i++) {
    evaluateLine(board, i, 14, 1, -1);
  }

  // std::cout << "Black: ";
  // for (int i=0; i<5; i++) {
  //   std::cout << blackScore[i] << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "White: ";
  // for (int i=0; i<5; i++) {
  //   std::cout << whiteScore[i] << " ";
  // }
  // std::cout << std::endl;

  if (currentPlayer == BLACK) {
    if (whiteScore[Five] > 0) {
      return -0.99;
    }
    if ((blackScore[Four] + blackScore[DeadFour]) > 0) {
      return 0.9;
    }
    if (whiteScore[Four] > 0) {
      return -0.9;
    }
    if (whiteScore[DeadFour] >= 2) {
      return -0.8;
    }
    if (whiteScore[DeadFour] > 0 && whiteScore[Three] > 0) {
      return 0.8;
    }
    if (blackScore[Three] > 0 && whiteScore[DeadFour] == 0) {
      return 0.8;
    }
  }
  else {
    if (blackScore[Five] > 0) {
      return 0.99;
    }
    if ((whiteScore[Four] + whiteScore[DeadFour]) > 0) {
      return -0.9;
    }
    if (blackScore[Four] > 0) {
      return 0.9;
    }
    if (blackScore[DeadFour] >= 2) {
      return 0.8;
    }
    if (blackScore[DeadFour] > 0 && blackScore[Three] > 0) {
      return 0.8;
    }
    if (whiteScore[Three] > 0 && blackScore[DeadFour] == 0) {
      return -0.8;
    }
  }

  int blackChessScore = blackScore[0] * 10000 + \
                        blackScore[1] * 10000 + \
                        blackScore[2] * 250 + \
                        blackScore[3] * 200 + \
                        blackScore[4] * 30 + \
                        blackScore[5] * 30;
  int whiteChessScore = whiteScore[0] * 10000 + \
                        whiteScore[1] * 10000 + \
                        whiteScore[2] * 250 + \
                        whiteScore[3] * 200 + \
                        whiteScore[4] * 30 + \
                        whiteScore[5] * 30;
  for (int i=0; i<15; i++) {
    for (int j=0; j<15; j++) {
      if (board[i][j] == BLACK) {
        blackChessScore += 7 - std::max(abs(i - 7), abs(j - 7));
      }
      else if (board[i][j] == WHITE) {
        whiteChessScore += 7 - std::max(abs(i - 7), abs(j - 7));
      }
    }
  }
  double score = blackChessScore - whiteChessScore;
  return tanh(score / 2000);
}

}  // namespace renju
}  // namespace open_spiel