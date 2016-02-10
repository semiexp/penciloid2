#include "sl_generator.h"

#include <vector>
#include <algorithm>
#include <random>

#include "sl_problem.h"
#include "sl_field.h"
#include "sl_clue_placement.h"
#include "sl_generator_option.h"
#include "../common/grid_loop_helper.h"

namespace penciloid
{
namespace slitherlink
{
namespace
{
bool HasUndecidedEdgeNearby(Field &field, Y y_base, X x_base)
{
	y_base = y_base * 2 + 1;
	x_base = x_base * 2 + 1;
	for (Y dy(-7); dy <= 7; ++dy) {
		for (X dx(-7); dx <= 7; ++dx) {
			Y y = y_base + dy;
			X x = x_base + dx;

			if (0 <= y && y <= 2 * field.height() && 0 <= x && x <= 2 * field.width()) {
				if (static_cast<int>(y) % 2 != static_cast<int>(x) % 2 && field.GetEdge(Position(y, x)) == Field::EDGE_UNDECIDED) {
					return true;
				}
			}
		}
	}
	return false;
}
bool HasZeroNearby(Field &field, Position pos)
{
	for (Y dy(-1); dy <= 1; ++dy) {
		for (X dx(-1); dx <= 1; ++dx) {
			Y y = pos.y + dy;
			X x = pos.x + dx;

			if (0 <= y && y < field.height() && 0 <= x && x < field.width() && field.GetClue(Position(y, x)) == Clue(0)) {
				return true;
			}
		}
	}

	return false;
}
}

bool GenerateByLocalSearch(const CluePlacement &placement, const GeneratorOption &constraint, std::mt19937 *rnd, Problem *ret)
{
	std::uniform_real_distribution<float> real_rnd(0.0, 1.0);

	Y height = placement.height();
	X width = placement.width();

	Problem current_problem(height, width);
	int max_step = static_cast<int>(height) * static_cast<int>(width) * 10;

	int number_unplaced_clues = 0;
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			Clue clue = placement.GetClue(Position(y, x));
			if (clue == kSomeClue) ++number_unplaced_clues;
			else if (clue != kNoClue) current_problem.SetClue(Position(y, x), clue);
		}
	}

	Field latest_field(current_problem, constraint.field_database);

	for (int step = 0; step < max_step; ++step) {
		Field::EdgeCount previous_decided_edges = latest_field.GetNumberOfDecidedEdges();
		bool is_progress = false;

		double temperature = 5.0;
		std::vector<Position> position_candidates;

		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) if (placement.GetClue(Position(y, x)) == kSomeClue) {
				if (HasUndecidedEdgeNearby(latest_field, y, x)) {
					position_candidates.push_back(Position(y, x));
				}
			}
		}

		std::shuffle(position_candidates.begin(), position_candidates.end(), *rnd);

		for (Position &pos : position_candidates) {
			Clue previous_clue = current_problem.GetClue(pos);
			bool is_zero_ok = !HasZeroNearby(latest_field, pos);

			std::vector<Clue> new_clue_candidates;
			for (Clue c(is_zero_ok ? 0 : 1); c <= 3; ++c) {
				if (c != previous_clue) new_clue_candidates.push_back(c);
			}

			std::shuffle(new_clue_candidates.begin(), new_clue_candidates.end(), *rnd);

			Field common;

			if (previous_clue == kNoClue) common = latest_field;
			else {
				current_problem.SetClue(pos, kNoClue);
				common = Field(current_problem, constraint.field_database);
			}

			for (Clue new_clue : new_clue_candidates) {
				current_problem.SetClue(pos, new_clue);

				Field next_field_candidate(common);
				next_field_candidate.AddClue(pos, new_clue);

				if (next_field_candidate.IsInconsistent()) continue;

				bool transition = false;
				Field::EdgeCount next_decided_edges = next_field_candidate.GetNumberOfDecidedEdges();

				if (previous_decided_edges < next_decided_edges) {
					transition = true;
				} else {
					double transition_probability = exp(static_cast<double>(static_cast<int>(next_decided_edges) - static_cast<int>(previous_decided_edges)) / temperature);
					if (real_rnd(*rnd) < transition_probability) transition = true;
				}

				if (!transition) continue;

				Field in_out_test_field = next_field_candidate;
				ApplyInOutRule(&in_out_test_field);
				CheckConnectability(&in_out_test_field);
				if (in_out_test_field.IsInconsistent()) {
					continue;
				}

				// update field & problem
				if (previous_clue == kNoClue) --number_unplaced_clues;
				previous_decided_edges = next_decided_edges;
				is_progress = true;
				latest_field = next_field_candidate;
				break;
			}

			if (is_progress) break;
			else current_problem.SetClue(pos, previous_clue);
		}

		if (latest_field.IsFullySolved() && !latest_field.IsInconsistent() && number_unplaced_clues == 0) {
			*ret = current_problem;
			return true;
		}
	}

	return false;
}
}
}
