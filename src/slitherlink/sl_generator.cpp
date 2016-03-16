#include "sl_generator.h"

#include <vector>
#include <algorithm>
#include <random>
#include <map>

#include "sl_problem.h"
#include "sl_field.h"
#include "sl_clue_placement.h"
#include "sl_generator_option.h"
#include "../common/grid_loop_helper.h"
#include "../common/union_find.h"

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
				if (static_cast<int>(y) % 2 != static_cast<int>(x) % 2 && field.GetEdge(LoopPosition(y, x)) == Field::kEdgeUndecided) {
					return true;
				}
			}
		}
	}
	return false;
}
bool HasZeroNearby(Field &field, CellPosition pos)
{
	for (Y dy(-1); dy <= 1; ++dy) {
		for (X dx(-1); dx <= 1; ++dx) {
			Y y = pos.y + dy;
			X x = pos.x + dx;

			if (0 <= y && y < field.height() && 0 <= x && x < field.width() && field.GetClue(CellPosition(y, x)) == Clue(0)) {
				return true;
			}
		}
	}

	return false;
}
long long FieldHash(Field &field, long long hash_size)
{
	long long ret = 0;
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			Clue c = field.GetClue(CellPosition(y, x));
			if (c == kNoClue) ret = (ret * 7) % hash_size;
			else ret = (ret * 7 + c + 1) % hash_size;
		}
	}
	return ret;
}
int CountProhibitedPattern(const CluePlacement &placement, Field &field)
{
	static const Direction kDirs[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	int ret = 0;
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			LoopPosition lp(y * 2 + 1, x * 2 + 1);
			if (placement.GetClue(CellPosition(y, x)) == kSomeClue && field.GetClue(CellPosition(y, x)) != 0 && HasZeroNearby(field, CellPosition(y, x))) {
				if (field.GetEdgeSafe(lp + kDirs[0]) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + kDirs[1]) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + kDirs[2]) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + kDirs[3]) == Field::kEdgeBlank) {
					++ret;
					goto nex;
				}
			}

			for (int d = 0; d < 4; ++d) {
				CellPosition pos2 = CellPosition(y, x) + kDirs[d];
				if (0 <= pos2.y && pos2.y < field.height() && 0 <= pos2.x && pos2.x < field.width() && placement.GetClue(pos2) == kSomeClue) goto nex;
			}

			if (field.GetClue(CellPosition(y, x)) == 2) {
				if (field.GetEdgeSafe(lp + Direction(Y(2), X(1))) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + Direction(Y(1), X(2))) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + Direction(Y(-2), X(-1))) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + Direction(Y(-1), X(-2))) == Field::kEdgeBlank) {
					++ret;
					goto nex;
				}
				if (field.GetEdgeSafe(lp + Direction(Y(-2), X(1))) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + Direction(Y(-1), X(2))) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + Direction(Y(2), X(-1))) == Field::kEdgeBlank &&
					field.GetEdgeSafe(lp + Direction(Y(1), X(-2))) == Field::kEdgeBlank) {
					++ret;
					goto nex;
				}
			} else if (placement.GetClue(CellPosition(y, x)) == kNoClue) {
				int n_in = 0, n_blank = 0;
				for (int d = 0; d < 4; ++d) {
					Direction dir1 = kDirs[d], dir2 = kDirs[(d + 1) % 4];
					auto edge1 = field.GetEdgeSafe(lp + dir1 * 2 + dir2);
					auto edge2 = field.GetEdgeSafe(lp + dir2 * 2 + dir1);

					if (edge1 == Field::kEdgeBlank && edge2 == Field::kEdgeBlank) ++n_blank;
					if (edge1 == Field::kEdgeBlank && edge2 == Field::kEdgeLine) ++n_in;
					if (edge1 == Field::kEdgeLine && edge2 == Field::kEdgeBlank) ++n_in;
				}

				if (n_in >= 1 && n_blank >= 2) {
					++ret;
				}
			}
		nex:
			continue;
		}
	}
	return ret;
}
}

bool GenerateByLocalSearch(const CluePlacement &placement, const GeneratorOption &constraint, std::mt19937 *rnd, Problem *ret)
{
	std::uniform_real_distribution<float> real_rnd(0.0, 1.0);

	Y height = placement.height();
	X width = placement.width();

	Problem current_problem(height, width);
	int max_step = static_cast<int>(height) * static_cast<int>(width) * 10;

	const int kTabuSize = 8;
	const long long kTabuHashSize = 1e9 + 7;
	long long tabu_list[kTabuSize];
	for (int i = 0; i < kTabuSize; ++i) tabu_list[i] = -1;
	std::map<long long, int> hash_count;

	int number_unplaced_clues = 0;
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			Clue clue = placement.GetClue(CellPosition(y, x));
			if (clue == kSomeClue) ++number_unplaced_clues;
			else if (clue != kNoClue) current_problem.SetClue(CellPosition(y, x), clue);
		}
	}

	Field latest_field(current_problem, constraint.field_database, constraint.method);
	Field::EdgeCount previous_decided_edges = 0;

	int no_progress = 0;
	int step = 0;

	for (; step < max_step; ++step) {
		// previous_decided_edges = latest_field.GetNumberOfDecidedEdges();
		bool is_progress = false;

		double temperature = 5.0;
		std::vector<CellPosition> position_candidates;

		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) if (placement.GetClue(CellPosition(y, x)) == kSomeClue) {
				if (latest_field.GetClue(CellPosition(y, x)) == kNoClue || HasUndecidedEdgeNearby(latest_field, y, x)) {
					position_candidates.push_back(CellPosition(y, x));
				}
			}
		}

		std::shuffle(position_candidates.begin(), position_candidates.end(), *rnd);

		for (CellPosition &pos : position_candidates) {
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
				common = Field(current_problem, constraint.field_database, constraint.method);
			}

			for (Clue new_clue : new_clue_candidates) {
				current_problem.SetClue(pos, new_clue);

				Field next_field_candidate(common);
				next_field_candidate.AddClue(pos, new_clue);

				if (next_field_candidate.IsInconsistent()) continue;

				long long next_hash = FieldHash(next_field_candidate, kTabuHashSize);
				bool tabu_collision = hash_count[next_hash] >= 5; // false;
				if (tabu_collision) continue;

				bool transition = false;
				Field::EdgeCount next_decided_edges = next_field_candidate.GetNumberOfDecidedEdges();
				next_decided_edges -= CountProhibitedPattern(placement, next_field_candidate) * 10;
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

				for (int i = 1; i < kTabuSize; ++i) tabu_list[i - 1] = tabu_list[i];
				tabu_list[kTabuSize - 1] = next_hash;
				hash_count[next_hash] += 1;

				break;
			}

			if (is_progress) break;
			else current_problem.SetClue(pos, previous_clue);
		}

		if (!is_progress) {
			++no_progress;
			if (no_progress >= 20) break;
		}
		if (latest_field.IsFullySolved() && !latest_field.IsInconsistent() && number_unplaced_clues == 0) {
			*ret = current_problem;
			return true;
		}
	}
	return false;
}

CluePlacement GenerateCluePlacement(Y height, X width, int number_clues, Symmetry symmetry, std::mt19937 *rnd)
{
	UnionFind cell_groups(static_cast<int>(height) * static_cast<int>(width));
	CluePlacement ret(height, width);

	auto cell_id = [height, width](Y y, X x) {
		return static_cast<int>(y) * static_cast<int>(width) + static_cast<int>(x);
	};

	if ((symmetry & kSymmetryTetrad) == kSymmetryTetrad && static_cast<int>(height) == static_cast<int>(width)) {
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				int id = cell_id(y, x);
				int id_rotated = cell_id(Y(x), X(height - y - 1));
				cell_groups.Join(id, id_rotated);
			}
		}
	}
	if (symmetry & kSymmetryDyad) {
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				int id = cell_id(y, x);
				int id_rotated = cell_id(height - y - 1, width - x - 1);
				cell_groups.Join(id, id_rotated);
			}
		}
	}
	if (symmetry & kSymmetryHorizontalLine) {
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				int id = cell_id(y, x);
				int id_flipped = cell_id(height - y - 1, x);
				cell_groups.Join(id, id_flipped);
			}
		}
	}
	if (symmetry & kSymmetryVerticalLine) {
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				int id = cell_id(y, x);
				int id_flipped = cell_id(y, width - x - 1);
				cell_groups.Join(id, id_flipped);
			}
		}
	}

	std::vector<std::pair<CellPosition, int> > possible_clues;

	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			int id = cell_id(y, x);
			if (cell_groups.Root(id) == id) {
				possible_clues.push_back({ CellPosition(y, x), cell_groups.UnionSize(id) });
			}
		}
	}
	while (number_clues > 0 && possible_clues.size() > 0) {
		std::vector<double> scores;
		double score_total = 0.0;

		for (auto p : possible_clues) {
			CellPosition pos = p.first;

			int score_base = 0;

			for (Y dy(-2); dy <= 2; ++dy) {
				for (X dx(-2); dx <= 2; ++dx) {
					CellPosition pos2 = pos + Direction(dy, dx);
					if (0 <= pos2.y && pos2.y < height && 0 <= pos2.x && pos2.x < width) {
						if (ret.GetClue(pos2) == kSomeClue) {
							int dist = 0;
							dist += (dy > 0 ? dy : -dy);
							dist += (dx > 0 ? dx : -dx);

							score_base += 5 - dist;
							if (dist == 1) score_base += 2;
						}
					}
				}
			}

			double cell_score = 64.0 * pow(2.0, (16.0 - score_base) / 2.0) + 4.0;
			scores.push_back(cell_score);
			score_total += cell_score;
		}

		double r = std::uniform_real_distribution<float>(0.0, static_cast<float>(score_total))(*rnd);
		int next_idx = possible_clues.size() - 1;

		for (int i = 0; i < scores.size(); ++i) {
			if (r < scores[i]) {
				next_idx = i;
				break;
			} else r -= scores[i];
		}

		CellPosition representative_pos = possible_clues[next_idx].first;
		int representative_id = cell_id(representative_pos.y, representative_pos.x);
		for (Y y(0); y < height; ++y) {
			for (X x(0); x < width; ++x) {
				int id = cell_id(y, x);
				if (cell_groups.Root(id) == cell_groups.Root(representative_id)) {
					--number_clues;
					ret.SetClue(CellPosition(y, x), kSomeClue);
				}
			}
		}

		possible_clues[next_idx] = possible_clues[possible_clues.size() - 1];
		possible_clues.pop_back();
	}

	return ret;
}
}
}
