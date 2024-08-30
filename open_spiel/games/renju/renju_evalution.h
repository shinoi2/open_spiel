#ifndef OPEN_SPIEL_GAMES_RENJU_EVALUTION_H_
#define OPEN_SPIEL_GAMES_RENJU_EVALUTION_H_

#include <vector>
#include <array>

namespace open_spiel {
namespace renju {

constexpr int Five = 0;
constexpr int Four = 1;
constexpr int DeadFour = 2;
constexpr int Three = 3;
constexpr int DeadThree = 4;
constexpr int Two = 5;
constexpr int UNKOWN = -1;

class RenjuEvalation {
public:
  RenjuEvalation() = default;
  double evaluate(const std::array<std::array<int, 15>, 15>& board);
  bool isForbidenMove(std::array<std::array<int, 15>, 15>& board, const int x, const int y);
private:
  void evaluateLine(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy);
  bool getChesses(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color, std::array<int, 6>& chesses);
  void evaluateFive(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color);
  void evaluateFour(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color);
  void evaluateDeadFour(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color);
  void evaluateThree(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color);
  void evaluateDeadThree(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color);
  void evaluateTwo(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color);
  void fillStatus(const std::array<std::array<int, 15>, 15>& board, const int x, const int y, const int dx, const int dy, const int color, const int cellState);
  std::array<std::array<int, 15>, 15> status;
  int currentPlayer;
  std::array<int, 6> blackScore;
  std::array<int, 6> whiteScore;
};

}  // namespace renju
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_RENJU_EVALUTION_H_
