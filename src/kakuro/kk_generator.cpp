#include "kk_generator.h"

#include "kk_answer.h"
#include "kk_field.h"
#include "../common/util.h"
#include "../common/mini_vector.h"

#include <vector>

namespace
{
int UndecidedCells(const penciloid::kakuro::Field &field)
{
	using namespace penciloid;
	int ret = 0;
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			if (field.GetCell(CellPosition(y, x)) == kakuro::Field::kCellUndecided) {
				++ret;
			}
		}
	}
	return ret;
}
double ComputeEnergy(const penciloid::kakuro::Field &field)
{
	using namespace penciloid;
	double ret = 0.0;
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			if (field.GetCell(CellPosition(y, x)) != kakuro::Field::kCellClue) {
				ret += PopCount(field.GetCandidateBits(CellPosition(y, x)));
			}
		}
	}
	return ret;
}
}
namespace penciloid
{
namespace kakuro
{
bool GenerateByLocalSearch(const Grid<bool> &is_clue, Dictionary *dic, std::mt19937 *rnd, Problem *ret)
{
	Y height = is_clue.height();
	X width = is_clue.width();
	Answer current_answer(height, width); double current_energy = 0.0;
	int max_step = static_cast<int>(height) * static_cast<int>(width) * 10;
	int step = 0;
	double temperature = 10.0;
	std::uniform_real_distribution<double> real_dist(0.0, 1.0);
	
	// TODO: initial answer
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if (!is_clue(y, x)) {
				current_answer.SetValue(CellPosition(y, x), 1 + (static_cast<int>(y) + static_cast<int>(x)) % 9);
			} else {
				current_answer.SetValue(CellPosition(y, x), Answer::kClueCell);
			}
		}
	}
	Problem problem = current_answer.ExtractProblem();
	Field field(problem, dic);
	field.CheckGroupAll();
	current_energy = ComputeEnergy(field);

	typedef MiniVector<std::pair<CellPosition, int>, 3> MoveCandidate;

	Field current_field = field;
	Grid<bool> modifiable(height, width, false);
	bool prev_fail = false;

	Grid<std::pair<int, int> > group_id(height, width, std::make_pair(-1, -1));
	std::vector<std::vector<CellPosition> > group_cells;
	std::vector<CellPosition> last_group;

	int group_id_last = -1;
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if (!is_clue(y, x)) {
				if (x == 0 || is_clue(y, x - 1)) {
					++group_id_last;
					if (!last_group.empty()) {
						group_cells.push_back(last_group);
						last_group.clear();
					}
				}
				group_id(y, x).first = group_id_last;
				last_group.push_back(CellPosition(y, x));
			}
		}
	}
	for (X x(0); x < width; ++x) {
		for (Y y(0); y < height; ++y) {
			if (!is_clue(y, x)) {
				if (y == 0 || is_clue(y - 1, x)) {
					++group_id_last;
					if (!last_group.empty()) {
						group_cells.push_back(last_group);
						last_group.clear();
					}
				}
				group_id(y, x).second = group_id_last;
				last_group.push_back(CellPosition(y, x));
			}
		}
	}
	if (!last_group.empty()) {
		group_cells.push_back(last_group);
		last_group.empty();
	}
	const CellPosition kUnusedValue = CellPosition(Y(-1), X(-1));
	AutoArray<CellPosition[10]> group_used_positions(group_id_last + 1);

	for (; step < max_step; ++step) {
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				CellPosition pos(y, x);
				if (current_answer.GetValue(pos) == Answer::kClueCell) continue;

				bool isok = false;
				if (current_field.GetCell(pos) == Field::kCellUndecided) isok = true;

				for (Direction d : k4Neighborhood) {
					for (CellPosition pos2 = pos + d; 0 <= pos2.y && pos2.y < height && 0 <= pos2.x && pos2.x < width; pos2 = pos2 + d) {
						if (current_answer.GetValue(pos2) == Answer::kClueCell) {
							break;
						}
						if (current_field.GetCell(pos2) == Field::kCellUndecided) {
							isok = true;
							break;
						}
					}
					if (isok) break;
				}
				modifiable(pos) = isok;
			}
		}
		for (int i = 0; i < group_id_last + 1; ++i) {
			for (int j = 1; j <= 9; ++j) group_used_positions[i][j] = kUnusedValue;
		}
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				int current_val = current_answer.GetValue(CellPosition(y, x));
				if (current_val == Answer::kClueCell) continue;
				std::pair<int, int> grp = group_id(y, x);

				group_used_positions[grp.first][current_val] = CellPosition(y, x);
				group_used_positions[grp.second][current_val] = CellPosition(y, x);
			}
		}
		std::vector<MoveCandidate> candidate;
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				CellPosition pos(y, x);
				int current_val = current_answer.GetValue(pos);
				if (current_val == Answer::kClueCell) continue;

				int group1 = group_id(y, x).first;
				int group2 = group_id(y, x).second;
				for (int n = 1; n <= 9; ++n) if (n != current_val) {
					CellPosition pos1 = group_used_positions[group1][n];
					CellPosition pos2 = group_used_positions[group2][n];

					if (pos1 == kUnusedValue && pos2 == kUnusedValue) {
						MoveCandidate mv;
						mv.push_back({ pos, n });
						candidate.push_back(mv);
					} else if (pos1 == kUnusedValue) {
						if (group_used_positions[group_id(pos).first][n] == kUnusedValue
							&& group_used_positions[group_id(pos2).first][current_val] == kUnusedValue
							&& (pos.x < pos2.x || pos.y < pos2.y)) {
							MoveCandidate mv;
							mv.push_back({ pos, n });
							mv.push_back({ pos2, current_val });
							candidate.push_back(mv);
						}
					} else if (pos2 == kUnusedValue) {
						if (group_used_positions[group_id(pos).second][n] == kUnusedValue
							&& group_used_positions[group_id(pos1).second][current_val] == kUnusedValue
							&& (pos.x < pos1.x || pos.y < pos1.y)) {
							MoveCandidate mv;
							mv.push_back({ pos, n });
							mv.push_back({ pos1, current_val });
							candidate.push_back(mv);
						}
					} else {
						if (group_used_positions[group_id(pos1).second][current_val] == kUnusedValue
							&& group_used_positions[group_id(pos2).first][current_val] == kUnusedValue) {
							MoveCandidate mv;
							mv.push_back({ pos, n });
							mv.push_back({ pos1, current_val });
							mv.push_back({ pos2, current_val });
							candidate.push_back(mv);
						}
					}
				}
			}
		}
		shuffle(candidate.begin(), candidate.end(), *rnd);
		bool fail = true;
		for (auto &c : candidate) {
			MiniVector<int, 3> previous_vals;
			bool has_modifiable_cell = false;
			for (int i = 0; i < c.size(); ++i) {
				previous_vals.push_back(current_answer.GetValue(c[i].first));
				has_modifiable_cell |= modifiable(c[i].first);
			}
			if (!has_modifiable_cell) {
				if (!prev_fail && real_dist(*rnd) < 0.9) continue;
			}
			for (int i = 0; i < c.size(); ++i) {
				current_answer.SetValue(c[i].first, c[i].second);
			}

			Problem problem = current_answer.ExtractProblem();
			Field field(problem, dic);
			field.CheckGroupAll();
			bool transition = false;

			if (!field.IsInconsistent()) {
				if (UndecidedCells(field) == 0) {
					*ret = problem;
					return true;
				}
				double next_energy = ComputeEnergy(field);
				if (current_energy > next_energy) {
					transition = true;
				}  else {
					double threshold = exp((current_energy - next_energy) / temperature);
					if (real_dist(*rnd) < threshold) transition = true;
				}

				if (transition) current_energy = next_energy;
			}

			if (transition) {
				current_field = field;
				fail = false;
				break;
			} else {
				for (int i = 0; i < c.size(); ++i) {
					current_answer.SetValue(c[i].first, previous_vals[i]);
				}
			}
		}
		prev_fail = fail;
		temperature *= 0.995;
	}

	return false;
}
}
}