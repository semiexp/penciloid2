#include "kk_generator.h"

#include "kk_answer.h"
#include "kk_field.h"
#include "../common/util.h"

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
bool FindInHorizontalGroup(const penciloid::kakuro::Answer &answer, penciloid::Y y, penciloid::X x, int val)
{
	using namespace penciloid;
	for (X x2(x - 1); x2 >= 0; --x2) {
		if (answer.GetValue(CellPosition(y, x2)) == kakuro::Field::kCellClue) break;
		if (answer.GetValue(CellPosition(y, x2)) == val) return true;
	}
	for (X x2(x + 1); x2 < answer.width(); ++x2) {
		if (answer.GetValue(CellPosition(y, x2)) == kakuro::Field::kCellClue) break;
		if (answer.GetValue(CellPosition(y, x2)) == val) return true;
	}
	return false;
}
bool FindInVerticalGroup(const penciloid::kakuro::Answer &answer, penciloid::Y y, penciloid::X x, int val)
{
	using namespace penciloid;
	for (Y y2(y - 1); y2 >= 0; --y2) {
		if (answer.GetValue(CellPosition(y2, x)) == kakuro::Field::kCellClue) break;
		if (answer.GetValue(CellPosition(y2, x)) == val) return true;
	}
	for (Y y2(y + 1); y2 < answer.height(); ++y2) {
		if (answer.GetValue(CellPosition(y2, x)) == kakuro::Field::kCellClue) break;
		if (answer.GetValue(CellPosition(y2, x)) == val) return true;
	}
	return false;
}

}
namespace penciloid
{
namespace kakuro
{
bool GenerateByLocalSearch(Problem &frame, Dictionary *dic, std::mt19937 *rnd, Problem *ret)
{
	Y height = frame.height();
	X width = frame.width();
	Answer current_answer(height, width); double current_energy = 0.0;
	int max_step = static_cast<int>(height) * static_cast<int>(width) * 10;
	int step = 0;
	double temperature = 10.0;
	std::uniform_real_distribution<double> real_dist(0.0, 1.0);
	
	// TODO: initial answer
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if (frame.GetClue(CellPosition(y, x)) == kEmptyCell) {
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

	struct MoveCandidate
	{
		bool is_swap;
		CellPosition pos1, pos2;
		int val1, val2;

		MoveCandidate(CellPosition pos, int val) : is_swap(false), pos1(pos), val1(val) {}
		MoveCandidate(CellPosition pos1, int val1, CellPosition pos2, int val2) :
			is_swap(true), pos1(pos1), pos2(pos2), val1(val1), val2(val2) {}
	};

	Field current_field = field;
	Grid<bool> modifiable(height, width, false);
	bool prev_fail = false;

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
				modifiable.at(pos) = isok;
			}
		}
		std::vector<MoveCandidate> candidate;
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				CellPosition pos(y, x);
				int current_val = current_answer.GetValue(pos);
				if (current_val == Answer::kClueCell) continue;

				for (int n = 1; n <= 9; ++n) if (n != current_val) {
					bool isok = true;
					for (Direction d : k4Neighborhood) {
						for (CellPosition pos2 = pos + d; 0 <= pos2.y && pos2.y < height && 0 <= pos2.x && pos2.x < width; pos2 = pos2 + d) {
							if (current_answer.GetValue(pos2) == n) {
								isok = false;
								break;
							} else if (current_answer.GetValue(pos2) == Answer::kClueCell) {
								break;
							}
						}
					}
					if (isok) {
						candidate.push_back(MoveCandidate(pos, n));
					}
				}

				// swap vertically
				for (Y y2(y + 1); y2 < height; ++y2) {
					if (current_answer.GetValue(CellPosition(y2, x)) == Answer::kClueCell) break;
					int current_val2 = current_answer.GetValue(CellPosition(y2, x));
					if (FindInHorizontalGroup(current_answer, y, x, current_val2) || FindInHorizontalGroup(current_answer, y2, x, current_val)) continue;

					candidate.push_back(MoveCandidate(pos, current_val2, CellPosition(y2, x), current_val));
				}

				// swap horizontally
				for (X x2(x + 1); x2 < width; ++x2) {
					if (current_answer.GetValue(CellPosition(y, x2)) == Answer::kClueCell) break;
					int current_val2 = current_answer.GetValue(CellPosition(y, x2));
					if (FindInVerticalGroup(current_answer, y, x, current_val2) || FindInVerticalGroup(current_answer, y, x2, current_val)) continue;

					candidate.push_back(MoveCandidate(pos, current_val2, CellPosition(y, x2), current_val));
				}
			}
		}
		shuffle(candidate.begin(), candidate.end(), *rnd);
		bool fail = true;
		for (auto c : candidate) {
			int previous_val1, previous_val2;
			if (!c.is_swap) {
				if (!modifiable.at(c.pos1)) {
					if (!prev_fail && real_dist(*rnd) < 0.9) continue;
				}
				previous_val1 = current_answer.GetValue(c.pos1);
				current_answer.SetValue(c.pos1, c.val1);
			} else {
				if (!modifiable.at(c.pos1) && !modifiable.at(c.pos2)) {
					if (!prev_fail && real_dist(*rnd) < 0.9) continue;
				}
				previous_val1 = current_answer.GetValue(c.pos1);
				current_answer.SetValue(c.pos1, c.val1);
				previous_val2 = current_answer.GetValue(c.pos2);
				current_answer.SetValue(c.pos2, c.val2);
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
				if (!c.is_swap) {
					current_answer.SetValue(c.pos1, previous_val1);
				} else {
					current_answer.SetValue(c.pos1, previous_val1);
					current_answer.SetValue(c.pos2, previous_val2);
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