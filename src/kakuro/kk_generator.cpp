#include "kk_generator.h"

#include "kk_answer.h"
#include "kk_field.h"

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
			//	ret += 1.0;
				ret += __popcnt(field.GetCandidateBits(CellPosition(y, x)));
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

	for (; step < max_step; ++step) {
		std::vector<std::pair<CellPosition, int> > candidate;
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
						candidate.push_back({ pos, n });
					}
				}
			}
		}
		shuffle(candidate.begin(), candidate.end(), *rnd);

		for (auto c : candidate) {
			CellPosition pos = c.first; int val = c.second;
			int previous_val = current_answer.GetValue(pos);
			current_answer.SetValue(pos, val);

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
				if (current_energy > next_energy) transition = true;
				else {
					double threshold = exp((current_energy - next_energy) / temperature);
					if (real_dist(*rnd) < threshold) transition = true;
				}

				if (transition) current_energy = next_energy;
			}

			if (transition) {
				break;
			} else {
				current_answer.SetValue(pos, previous_val);
			}
		}
		temperature *= 0.995;
	}

	return false;
}
}
}