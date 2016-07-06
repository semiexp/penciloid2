#include "ms_generator.h"

#include <vector>
#include <algorithm>
#include <cmath>

#include "ms_field.h"
#include "../common/grid_loop_helper.h"

namespace
{
double ComputeEnergy(penciloid::masyu::Field &field)
{
	using namespace penciloid;
	int n_clue = 0;
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			if (field.GetClue(CellPosition(y, x)) != masyu::kNoClue) ++n_clue;
		}
	}
	return 8.0 * n_clue - (double)field.GetNumberOfDecidedEdges();
}
}

namespace penciloid
{
namespace masyu
{
bool GenerateByLocalSearch(Y height, X width, std::mt19937 *rnd, Problem *ret)
{
	Problem current_problem(height, width); double current_energy = 0.0;
	int max_step = static_cast<int>(height) * static_cast<int>(width) * 10;
	int step = 0;
	double temperature = 10.0;
	std::uniform_real_distribution<double> real_dist(0.0, 1.0);

	for (; step < max_step; ++step) {
		std::vector<std::pair<CellPosition, Clue> > candidate;
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				CellPosition pos(y, x);
				Clue current_clue = current_problem.GetClue(pos);

				if (current_clue != kNoClue) candidate.push_back({ pos, kNoClue });
				if (current_clue != kClueBlack) candidate.push_back({ pos, kClueBlack });
				if (current_clue != kClueWhite) candidate.push_back({ pos, kClueWhite });
			}
		}
		shuffle(candidate.begin(), candidate.end(), *rnd);

		for (auto c : candidate) {
			CellPosition pos = c.first; Clue clue = c.second;
			Clue previous_clue = current_problem.GetClue(pos);
			current_problem.SetClue(pos, clue);
			
			Field next_field(current_problem);
			bool transition = false;

			if (!next_field.IsInconsistent()) {
				ApplyInOutRule(&next_field);
				CheckConnectability(&next_field);

				if (!next_field.IsInconsistent()) {
					if (next_field.IsFullySolved()) {
						*ret = current_problem;
						return true;
					}
					double next_energy = ComputeEnergy(next_field);
					if (current_energy > next_energy) transition = true;
					else {
						double threshold = exp((current_energy - next_energy) / temperature);
						if (real_dist(*rnd) < threshold) transition = true;
					}

					if (transition) current_energy = next_energy;
				}
			}

			if (transition) {
				break;
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