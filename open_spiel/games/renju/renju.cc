// Copyright 2019 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "open_spiel/games/renju/renju.h"
#include "open_spiel/games/renju/renju_evalution.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "open_spiel/spiel_utils.h"
#include "open_spiel/utils/tensor_view.h"

namespace open_spiel {
namespace renju {
namespace {

// Facts about the game.
const GameType kGameType{
    /*short_name=*/"renju",
    /*long_name=*/"Renju",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kDeterministic,
    GameType::Information::kPerfectInformation,
    GameType::Utility::kZeroSum,
    GameType::RewardModel::kTerminal,
    /*max_num_players=*/2,
    /*min_num_players=*/2,
    /*provides_information_state_string=*/true,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/true,
    /*provides_observation_tensor=*/true,
    /*parameter_specification=*/
    {
      {"players", GameParameter(GameParameter::Type::kInt)},
      {"colors", GameParameter(GameParameter::Type::kInt)},
      {"ranks", GameParameter(GameParameter::Type::kInt)},
      {"hand_size", GameParameter(GameParameter::Type::kInt)},
      {"max_information_tokens", GameParameter(GameParameter::Type::kInt)},
      {"max_life_tokens", GameParameter(GameParameter::Type::kInt)},
      {"seed", GameParameter(GameParameter::Type::kInt)},
      {"random_start_player", GameParameter(GameParameter::Type::kBool)},
      {"observation_type", GameParameter(GameParameter::Type::kString)},
    }
};

std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::shared_ptr<const Game>(new RenjuGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);

RegisterSingleTensorObserver single_tensor(kGameType.short_name);

}  // namespace

CellState PlayerToState(Player player) {
  switch (player) {
    case 0:
      return CellState::kBlack;
    case 1:
      return CellState::kWhite;
    default:
      SpielFatalError(absl::StrCat("Invalid player id ", player));
      return CellState::kEmpty;
  }
}

std::string StateToString(CellState state) {
  switch (state) {
    case CellState::kEmpty:
      return ".";
    case CellState::kWhite:
      return "o";
    case CellState::kBlack:
      return "x";
    default:
      SpielFatalError("Unknown state.");
  }
}

bool RenjuState::HasLine(Player player) const {
  CellState c = PlayerToState(player);
  for (int col = 0; col < kNumCols; ++col) {
    for (int row = 0; row < kNumRows; ++row) {
      if (BoardAt(row, col) == c && HasLineFrom(player, row, col)) return true;
    }
  }
  return false;
}

bool RenjuState::HasLineFrom(Player player, int row, int col) const {
  return HasLineFromInDirection(player, row, col, 0, 1) ||
         HasLineFromInDirection(player, row, col, -1, -1) ||
         HasLineFromInDirection(player, row, col, -1, 0) ||
         HasLineFromInDirection(player, row, col, -1, 1);
}

bool RenjuState::HasLineFromInDirection(Player player, int row,
                                        int col, int drow,
                                        int dcol) const {
  if (row + 4 * drow >= kNumRows || col + 4 * dcol >= kNumCols ||
      row + 4 * drow < 0 || col + 4 * dcol < 0)
    return false;
  CellState c = PlayerToState(player);
  for (int i = 0; i < 5; ++i) {
    if (BoardAt(row, col) != c) return false;
    row += drow;
    col += dcol;
  }
  return true;
}

void RenjuState::DoApplyAction(Action move) {
  SPIEL_CHECK_EQ(board_[move], CellState::kEmpty);
  board_[move] = PlayerToState(CurrentPlayer());
  if (HasLine(current_player_)) {
    outcome_ = current_player_;
  }
  current_player_ = 1 - current_player_;
  num_moves_ += 1;
}

std::vector<Action> RenjuState::LegalActions() const {
  if (IsTerminal()) return {};
  // Can move in any empty cell.
  std::vector<Action> moves;
  std::array<std::array<int, 16>, 16> preSum;
  for (int i=1; i<=15; i++) {
    for (int j=1; j<=15; j++) {
      preSum[i][j] = preSum[i-1][j] + preSum[i][j-1] - preSum[i-1][j-1];
      if (BoardAt(i-1, j-1) != CellState::kEmpty) {
        preSum[i][j] ++;
      }
    }
  }

  for (int cell = 0; cell < kNumCells; ++cell) {
    if (board_[cell] == CellState::kEmpty) {
      int x = cell / 15;
      int y = cell % 15;
      int lx = std::max(0, x-2);
      int ly = std::max(0, y-2);
      int rx = std::min(15, x+3);
      int ry = std::min(15, y+3);
      if ((preSum[rx][ry] - preSum[lx][ry] - preSum[rx][ly] + preSum[lx][ly]) > 0) {
        moves.push_back(cell);
      }
    }
  }
  if (moves.empty() && BoardAt(7, 7) == CellState::kEmpty) {
    moves.push_back(112);
  }
  return moves;
}

std::string RenjuState::ActionToString(Player player,
                                           Action action_id) const {
  return game_->ActionToString(player, action_id);
}

bool RenjuState::IsFull() const { return num_moves_ == kNumCells; }

RenjuState::RenjuState(std::shared_ptr<const Game> game) : State(game) {
  std::fill(begin(board_), end(board_), CellState::kEmpty);
}

std::string RenjuState::ToString() const {
  std::string str;
  for (int r = 0; r < kNumRows; ++r) {
    for (int c = 0; c < kNumCols; ++c) {
      absl::StrAppend(&str, StateToString(BoardAt(r, c)));
    }
    if (r < (kNumRows - 1)) {
      absl::StrAppend(&str, "\n");
    }
  }
  return str;
}

bool RenjuState::IsTerminal() const {
  return outcome_ != kInvalidPlayer || IsFull();
}

std::vector<double> RenjuState::Returns() const {
  if (HasLine(Player{0})) {
    return {1.0, -1.0};
  } else if (HasLine(Player{1})) {
    return {-1.0, 1.0};
  } else {
    return {0.0, 0.0};
  }
}

std::string RenjuState::InformationStateString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  return HistoryString();
}

std::string RenjuState::ObservationString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);
  return ToString();
}

void RenjuState::ObservationTensor(Player player,
                                       absl::Span<float> values) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);

  // Treat `values` as a 2-d tensor.
  TensorView<2> view(values, {kCellStates, kNumCells}, true);
  for (int cell = 0; cell < kNumCells; ++cell) {
    view[{static_cast<int>(board_[cell]), cell}] = 1.0;
  }
}

void RenjuState::UndoAction(Player player, Action move) {
  board_[move] = CellState::kEmpty;
  current_player_ = player;
  outcome_ = kInvalidPlayer;
  num_moves_ -= 1;
  history_.pop_back();
  --move_number_;
}

std::unique_ptr<State> RenjuState::Clone() const {
  return std::unique_ptr<State>(new RenjuState(*this));
}

std::string RenjuGame::ActionToString(Player player,
                                          Action action_id) const {
  return absl::StrCat(StateToString(PlayerToState(player)), "(",
                      action_id / kNumCols, ",", action_id % kNumCols, ")");
}

RenjuGame::RenjuGame(const GameParameters& params)
    : Game(kGameType, params) {}

std::vector<double> RenjuState::Rewards() const {
  if (IsTerminal()) {
    return Returns();
  }
  std::array<std::array<int, 15>, 15> board;
  for (int i=0; i<15; i++) {
    for (int j=0; j<15; j++) {
      if (BoardAt(i, j) == CellState::kEmpty) {
        board[i][j] = 0;
      } else if (BoardAt(i, j) == CellState::kBlack) {
        board[i][j] = 1;
      } else if (BoardAt(i, j) == CellState::kWhite) {
        board[i][j] = 2;
      }
    }
  }
  RenjuEvalation evalation;
  double score = evalation.evaluate(board);
  return {score, -score};
}

}  // namespace renju
}  // namespace open_spiel

