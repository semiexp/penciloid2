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
void UpdateClueChangeCandidate(penciloid::Grid<bool> &candidate, penciloid::nurikabe::Field &field, penciloid::CellPosition pos)
{
	using namespace penciloid;
	using namespace nurikabe;
	if (!candidate.IsPositionOnGrid(pos) || field.GetCell(pos) == Field::kCellBlack || candidate(pos)) return;
	candidate(pos) = true;
	for (Direction d : k4Neighborhood) {
		UpdateClueChangeCandidate(candidate, field, pos + d);
	}
}
int ProblemHash(penciloid::nurikabe::Problem &problem)
{
	using namespace penciloid;
	using namespace nurikabe;

	const long long kHashMul = 1009, kHashMod = 1083441539;
	long long ret = 0;
	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			ret = ret * kHashMul * kHashMul % kHashMod;
			Clue c = problem.GetClue(CellPosition(y, x));
			if (c != kNoClue) {
				ret = (ret + c.clue_low * kHashMul + c.clue_high) % kHashMod;
			}
		}
	}
	return static_cast<int>(ret);
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

	const int kTabuListSize = 9;
	int tabu_list[kTabuListSize], tabu_list_end = 0;
	for (int i = 0; i < kTabuListSize; ++i) tabu_list[i] = -1;

	int n_clue = 0;
	int clue_max = static_cast<int>(height) + static_cast<int>(width);

	// add an arbitrary clue (completely empty problem will cause inconsistency)
	Y initial_y = Y(std::uniform_int_distribution<int>(0, static_cast<int>(height) - 1)(*rnd));
	X initial_x = X(std::uniform_int_distribution<int>(0, static_cast<int>(width) - 1)(*rnd));
	current_problem.SetClue(CellPosition(initial_y, initial_x), Clue(1, clue_max));
	++n_clue;

	Field current_field(current_problem);

	for (; step < max_step; ++step) {
		printf("%d (%f, %f)\n", step, current_energy, temperature);
		std::cout << current_field << std::endl;
		
		Grid<bool> clue_change_candidate(height, width);
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				if (current_field.GetCell(CellPosition(y, x)) != Field::kCellUndecided) continue;
				for (int dy = -2; dy <= 2; ++dy) {
					for (int dx = -2; dx <= 2; ++dx) {
						if (abs(dy) + abs(dx) > 2) continue;
						CellPosition pos2(y + dy, x + dx);
						if (clue_change_candidate.IsPositionOnGrid(pos2) && current_field.GetCell(pos2) != Field::kCellUndecided) {
							UpdateClueChangeCandidate(clue_change_candidate, current_field, pos2);
						}
					}
				}
			}
		}
		std::vector<std::pair<CellPosition, Clue> > candidate;
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				CellPosition pos(y, x);
				Clue current_clue = current_problem.GetClue(pos);
				if (current_clue != kNoClue) {
					if (!clue_change_candidate(pos)) continue;
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

			int hash = ProblemHash(current_problem);
			bool is_tabu = false;
			for (int i = 0; i < kTabuListSize; ++i) {
				if (tabu_list[i] == hash) {
					is_tabu = true; break;
				}
			}
			if (is_tabu) {
				current_problem.SetClue(pos, previous_clue);
				continue;
			}

			Field next_field(current_problem);
			bool transition = false;

			next_field.Solve();
			next_field.CheckConsistency();
			double next_energy = ComputeEnergy(next_field);
			if (!next_field.IsInconsistent()) {
				if (next_field.IsFullySolved()) {
					next_field.RestrictClueOfClosedGroups();
					for (Y y(0); y < height; ++y) {
						for (X x(0); x < width; ++x) {
							Clue clue = next_field.GetClue(CellPosition(y, x));
							if (clue != kNoClue) {
								current_problem.SetClue(CellPosition(y, x), clue);
							}
						}
					}
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
				tabu_list[tabu_list_end] = ProblemHash(current_problem);
				if (++tabu_list_end == kTabuListSize) tabu_list_end = 0;

				current_field = next_field;
				current_energy = next_energy;
				if (previous_clue != kNoClue) --n_clue;
				if (clue != kNoClue) ++n_clue;
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
