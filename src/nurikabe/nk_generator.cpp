#include "nk_generator.h"

#include <random>
#include <vector>

#include "nk_field.h"

namespace
{
double ComputeEnergy(penciloid::nurikabe::Field &field)
{
	using namespace penciloid;
	using namespace nurikabe;

	double energy = 0;
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			auto status = field.GetCell(CellPosition(y, x));
			if (status != Field::kCellUndecided) energy -= 1.0;
			if (status == Field::kCellWhite) {
				Clue clue = field.GetClue(CellPosition(y, x));
				if (clue != kNoClue) {
					energy += 5.0 / (clue.clue_high - clue.clue_low + 1) + (clue.clue_low < 5 ? 4.0 * (5 - clue.clue_low) : 0.0);
				}
			}
		}
	}
	return energy;
}
}

namespace penciloid
{
namespace nurikabe
{
bool GenerateByLocalSearch(Y height, X width, std::mt19937 *rnd, Problem *ret)
{
	Problem current_problem(height, width); double current_energy = 0.0;
	int max_step = static_cast<int>(height) * static_cast<int>(width) * 10;
	int step = 0;
	double temperature = 10.0;
	std::uniform_real_distribution<double> real_dist(0.0, 1.0);

	int n_clue = 0;
	int clue_max = static_cast<int>(height) + static_cast<int>(width);

	// add an arbitrary clue (completely empty problem will cause inconsistency)
	Y initial_y = Y(std::uniform_int_distribution<int>(0, static_cast<int>(height) - 1)(*rnd));
	X initial_x = X(std::uniform_int_distribution<int>(0, static_cast<int>(width) - 1)(*rnd));
	current_problem.SetClue(CellPosition(initial_y, initial_x), Clue(1, clue_max));
	++n_clue;

	Field current_field(current_problem);

	for (; step < max_step; ++step) {
		std::vector<std::pair<CellPosition, Clue> > candidate;
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				CellPosition pos(y, x);
				Clue current_clue = current_problem.GetClue(pos);
				if (current_clue != kNoClue) {
					if (n_clue > 1) {
						candidate.push_back({ pos, kNoClue });
					}
					if (current_clue.clue_high - current_clue.clue_low >= 1) {
						candidate.push_back({ pos, Clue(current_clue.clue_low + 1, current_clue.clue_high) });
						candidate.push_back({ pos, Clue(current_clue.clue_low, current_clue.clue_high - 1) });
					}
					if (current_clue.clue_low > 3) {
						candidate.push_back({ pos, Clue(current_clue.clue_low - 1, current_clue.clue_high) });
					}
					if (current_clue.clue_high < clue_max) {
						candidate.push_back({ pos, Clue(current_clue.clue_low, current_clue.clue_high + 1) });
					}
				} else {
					if (current_field.GetCell(pos) == Field::kCellUndecided) {
						candidate.push_back({ pos, Clue(3, clue_max) });
					}
				}
			}
		}
		std::shuffle(candidate.begin(), candidate.end(), *rnd);

		for (auto c : candidate) {
			CellPosition pos = c.first; Clue clue = c.second;
			Clue previous_clue = current_problem.GetClue(pos);
			current_problem.SetClue(pos, clue);

			Field next_field(current_problem);
			bool transition = false;

			next_field.Solve();
			next_field.CheckConsistency();
			double next_energy = ComputeEnergy(next_field);
			if (!next_field.IsInconsistent()) {
				if (next_field.IsFullySolved()) {
					*ret = current_problem;
					return true;
				}
				if (current_energy > next_energy) transition = true;
				else {
					double threshold = exp((current_energy - next_energy) / temperature);
					if (real_dist(*rnd) < threshold) transition = true;
				}
			}

			if (transition) {
				current_field = next_field;
				current_energy = next_energy;
				if (previous_clue != kNoClue) --n_clue;
				if (clue != kNoClue) ++n_clue;
			} else {
				current_problem.SetClue(pos, previous_clue);
			}
		}

		temperature *= 0.995;
	}

	return false;
}
}
}
