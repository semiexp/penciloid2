#include "sl_evaluator.h"

#include <algorithm>

#include "../common/union_find.h"
#include "sl_method.h"

namespace penciloid
{
namespace slitherlink
{
const double Evaluator::kScoreImpossible = -1.0;
const double Evaluator::kScoreInconsistent = -2.0;

Evaluator::Evaluator() : field_(), param_(), move_candidates_()
{
}
Evaluator::Evaluator(Problem &problem)// : field_(), param_(), move_candidates_()
{
	Method method;
	method.DisableAll();
	field_ = Field(problem, nullptr, method);
}
Evaluator::~Evaluator()
{
}
double Evaluator::Evaluate()
{
	double score = 0.0;
	std::vector<LoopPosition> last_pos;

	int height_as_int = static_cast<int>(field_.height());
	int width_as_int = static_cast<int>(field_.width());
	int number_of_total_edges = (height_as_int + 1) * width_as_int + height_as_int * (width_as_int + 1);

	while (!field_.IsInconsistent() && !field_.IsFullySolved()) {
		move_candidates_.clear();
		EnumerateMoves();
		if (move_candidates_.size() == 0) {
		//	std::cout << field_ << std::endl;
			return kScoreImpossible;
		}

		std::vector<ScoredMove> scored_candidates;
		for (Move &m : move_candidates_) {
			scored_candidates.push_back(ScoredMove(GetScoreOfMethod(m.method), m));
		}

		for (ScoredMove &m : scored_candidates) {
			// These methods don't involve locality
			if (m.move.method == kAdjacent3s || m.move.method == kDiagonal3s || m.move.method == kAdjacentLines0) continue;

			int locality_distance = 2 * (static_cast<int>(field_.height()) + static_cast<int>(field_.width()));
			for (LoopPosition pos : m.move.target_pos) {
				for (LoopPosition pos2 : last_pos) {
					int d = abs(static_cast<int>(pos.y - pos2.y)) + abs(static_cast<int>(pos.x - pos2.x));
					locality_distance = std::min(locality_distance, d);
				}
			}
			double locality_weight = pow(param_.locality_base, std::min(1.0, (locality_distance - 1) / 4.0) - 1);
			m.score *= locality_weight;
		}

		double current_score = 0.0;
		bool is_score_zero = false;
		for (ScoredMove &m : scored_candidates) {
			if (m.score < 1e-6) {
				is_score_zero = true;
				break;
			} else {
				current_score += pow(m.score / m.move.target_pos.size(), -param_.alternative_dimension);
			}
		}

		if (is_score_zero) current_score = 0.0;
		else {
			current_score = pow(current_score, -1.0 / param_.alternative_dimension);
		}
		current_score *= sqrt(number_of_total_edges - field_.GetNumberOfDecidedEdges());

		int easiest_move_index = 0;
		double easiest_move_score = 1e10;

		for (int i = 0; i < scored_candidates.size(); ++i) {
			double score_tmp = scored_candidates[i].score / scored_candidates[i].move.target_pos.size();

			if (easiest_move_score > score_tmp) {
				easiest_move_score = score_tmp;
				easiest_move_index = i;
			}
		}

		Move &next_move = scored_candidates[easiest_move_index].move;
		score += current_score * next_move.target_pos.size();

		for (int i = 0; i < next_move.target_pos.size(); ++i) {
			field_.DecideEdge(next_move.target_pos[i], next_move.target_state[i]);
		}
		last_pos = next_move.target_pos;
	}
	
	if (!field_.IsFullySolved()) return kScoreInconsistent;
	return score;
}
void Evaluator::EnumerateMoves()
{
	CheckTwoLinesRule();
	CheckAvoidCycleRule();
	CheckTheoremsAbout3();

	for (Y y(0); y <= height(); ++y) {
		for (X x(0); x <= width(); ++x) {
			CheckHourglassRule(LoopPosition(y * 2, x * 2));
		}
	}
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			CellPosition pos(y, x);
			if (!CheckAdjacentLinesRule(pos)) {
				CheckCornerCell(pos);
				CheckLineToClue(pos);
				CheckAlmostLineTo2(pos);
				CheckLineFromClue(pos);
				CheckDiagonalChain(pos);
			}
		}
	}

	CheckInOutRule();
	EliminateDoneMoves();
}
void Evaluator::EliminateDoneMoves()
{
	std::vector<Move> new_move_candidates;
	
	for (Move &move : move_candidates_) {
		Move new_move(move.method);
		for (int i = 0; i < move.target_pos.size(); ++i) {
			if (GetEdgeSafe(move.target_pos[i]) == kEdgeUndecided) {
				new_move.target_pos.push_back(move.target_pos[i]);
				new_move.target_state.push_back(move.target_state[i]);
			}
		}
		if (new_move.target_pos.size() > 0) new_move_candidates.push_back(new_move);
	}

	move_candidates_.swap(new_move_candidates);
}
double Evaluator::GetScoreOfMethod(AppliedMethod method)
{
	switch (method)
	{
	case kTwoLines: return param_.two_lines;
	case kAvoidCycle: return param_.avoid_cycle;
	case kHourglassRule: return param_.hourglass_rule;
	case kAdjacentLines0: return param_.adjacent_lines[0];
	case kAdjacentLines1: return param_.adjacent_lines[1];
	case kAdjacentLines2: return param_.adjacent_lines[2];
	case kAdjacentLines3: return param_.adjacent_lines[3];
	case kAdjacent3s: return param_.adjacent_3;
	case kDiagonal3s: return param_.diagonal_3;
	case kDiagonal3sAvoidCycle: return param_.diagonal_3_avoid_cycle;
	case kCornerClue1: return param_.corner_clue[1];
	case kCornerClue2: return param_.corner_clue[2];
	case kCornerClue3: return param_.corner_clue[3];
	case kLineToClue1: return param_.line_to_clue[1];
	case kLineToClue2: return param_.line_to_clue[2];
	case kLineToClue3: return param_.line_to_clue[3];
	case kLineFromClue1: return param_.line_from_clue[1];
	case kLineFromClue3: return param_.line_from_clue[3];
	case kAlmostLineTo2: return param_.almost_line_to_2;
	case kDiagonalChain: return param_.diagonal_chain;
	case kInoutRule: return param_.inout_rule;
	}
	return -1.0;
}
void Evaluator::CheckTwoLinesRule()
{
	for (Y y(0); y <= height() * 2; y += 2) {
		for (X x(0); x <= width() * 2; x += 2) {
			int n_lines = 0;

			for (Direction d : k4Neighborhood) {
				if (GetEdgeSafe(LoopPosition(y, x) + d) == kEdgeLine) ++n_lines;
			}
			if (n_lines == 2) {
				for (Direction d : k4Neighborhood) {
					if (GetEdgeSafe(LoopPosition(y, x) + d) == kEdgeUndecided) {
						move_candidates_.push_back(Move(LoopPosition(y, x) + d, kEdgeBlank, kTwoLines));
					}
				}
			}
		}
	}
}
void Evaluator::CheckAvoidCycleRule()
{
	for (Y y(0); y <= height() * 2; y += 2) {
		for (X x(0); x <= width() * 2; x += 2) {
			LoopPosition pos(y, x);
			LoopPosition line_destination;
			Field::EdgeCount line_weight;
			int n_lines = 0;

			for (Direction d : k4Neighborhood) {
				if (GetEdgeSafe(pos + d) == kEdgeLine) {
					++n_lines;
					line_destination = field_.GetAnotherEnd(pos, d);
					line_weight = field_.GetChainLength(pos, d);
				}
			}

			if (n_lines != 1 || line_weight == field_.GetNumberOfDecidedLines()) continue;

			for (Direction d : k4Neighborhood) {
				if (GetEdgeSafe(pos + d) == kEdgeUndecided && field_.GetAnotherEnd(pos, d) == line_destination) {
					move_candidates_.push_back(Move(pos + d, kEdgeBlank, kAvoidCycle));
				}
			}
		}
	}
}
void Evaluator::CheckHourglassRule(LoopPosition pos)
{
	std::vector<int> line, undecided;
	int line_weight = 0;

	for (int d = 0; d < 4; ++d) {
		EdgeState state = GetEdgeSafe(pos + k4Neighborhood[d]);
		if (state == kEdgeLine) {
			line.push_back(d);
			line_weight = field_.GetChainLength(pos, k4Neighborhood[d]);
		} else if (state == kEdgeUndecided) {
			undecided.push_back(d);
		}
	}
	if (!(line.size() == 1 && undecided.size() == 2)) return;

	LoopPosition line_companion = field_.GetAnotherEnd(pos, k4Neighborhood[line[0]]);
	LoopPosition undecided_target0 = field_.GetAnotherEnd(pos, k4Neighborhood[undecided[0]]);
	LoopPosition undecided_target1 = field_.GetAnotherEnd(pos, k4Neighborhood[undecided[1]]);

	// Are undecided_target0 and undecided_target1 connected by a chain of lines?
	bool is_line_chain = false;
	for (Direction d : k4Neighborhood) {
		if (GetEdgeSafe(undecided_target0 + d) == kEdgeLine && field_.GetAnotherEnd(undecided_target0, d) == undecided_target1) {
			is_line_chain = true;
			line_weight += field_.GetChainLength(undecided_target0, d);
		}
	}

	if (!is_line_chain || line_weight >= field_.GetNumberOfDecidedLines()) return;

	for (LoopPosition base : {undecided_target0, undecided_target1}){
		for (Direction d : k4Neighborhood) {
			if (GetEdgeSafe(base + d) == kEdgeUndecided && field_.GetAnotherEnd(base, d) == line_companion) {
				move_candidates_.push_back(Move(base + d, kEdgeBlank, kHourglassRule));
			}
		}
	}
}
void Evaluator::CheckTheoremsAbout3()
{
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			if (field_.GetClue(CellPosition(y, x)) != 3) continue;
			LoopPosition loop_pos(2 * y + 1, 2 * x + 1);

			if (y != height() - 1 && field_.GetClue(CellPosition(y + 1, x)) == 3) {
				Move m(kAdjacent3s);
				m.AddTarget(loop_pos + Direction(Y(-1), X(0)), kEdgeLine);
				m.AddTarget(loop_pos + Direction(Y(1), X(0)), kEdgeLine);
				m.AddTarget(loop_pos + Direction(Y(3), X(0)), kEdgeLine);
				m.AddTarget(loop_pos + Direction(Y(1), X(-2)), kEdgeBlank);
				m.AddTarget(loop_pos + Direction(Y(1), X(2)), kEdgeBlank);
				move_candidates_.push_back(m);
			}
			if (x != width() - 1 && field_.GetClue(CellPosition(y, x + 1)) == 3) {
				Move m(kAdjacent3s);
				m.AddTarget(loop_pos + Direction(Y(0), X(-1)), kEdgeLine);
				m.AddTarget(loop_pos + Direction(Y(0), X(1)), kEdgeLine);
				m.AddTarget(loop_pos + Direction(Y(0), X(3)), kEdgeLine);
				m.AddTarget(loop_pos + Direction(Y(-2), X(1)), kEdgeBlank);
				m.AddTarget(loop_pos + Direction(Y(2), X(1)), kEdgeBlank);
				move_candidates_.push_back(m);
			}
			for (int sgn : {-1, 1}) {
				if (y != height() - 1 && 0 <= x + sgn && x + sgn < width() && field_.GetClue(CellPosition(y + 1, x + sgn)) == 3) {
					Move m(kDiagonal3s);
					m.AddTarget(loop_pos + Direction(Y(0), X(-sgn)), kEdgeLine);
					m.AddTarget(loop_pos + Direction(Y(-1), X(0)), kEdgeLine);
					m.AddTarget(loop_pos + Direction(Y(2), X(3 * sgn)), kEdgeLine);
					m.AddTarget(loop_pos + Direction(Y(3), X(2 * sgn)), kEdgeLine);
					move_candidates_.push_back(m);

					if (GetEdgeSafe(loop_pos + Direction(Y(1), X(0))) == kEdgeUndecided && GetEdgeSafe(loop_pos + Direction(Y(0), X(sgn))) == kEdgeUndecided &&
						GetEdgeSafe(loop_pos + Direction(Y(1), X(2 * sgn))) == kEdgeUndecided && GetEdgeSafe(loop_pos + Direction(Y(2), X(sgn))) == kEdgeUndecided &&
						field_.GetNumberOfDecidedLines() > 4) {
						if (field_.GetAnotherEnd(loop_pos + Direction(Y(-1), X(sgn)), Direction(Y(0), X(sgn))) == loop_pos + Direction(Y(1), X(3 * sgn))) {
							move_candidates_.push_back(Move(loop_pos + Direction(Y(-1), X(2 * sgn)), kEdgeBlank, kDiagonal3sAvoidCycle));
						}
						if (field_.GetAnotherEnd(loop_pos + Direction(Y(1), X(-sgn)), Direction(Y(1), X(0))) == loop_pos + Direction(Y(3), X(sgn))) {
							move_candidates_.push_back(Move(loop_pos + Direction(Y(2), X(-sgn)), kEdgeBlank, kDiagonal3sAvoidCycle));
						}
					}
				}
			}
		}
	}
}
bool Evaluator::CheckAdjacentLinesRule(CellPosition pos)
{
	if (field_.GetClue(pos) == kNoClue) return false;

	Clue clue = field_.GetClue(pos);
	LoopPosition loop_pos(pos.y * 2 + 1, pos.x * 2 + 1);
	int n_lines = 0, n_blanks = 0;

	for (Direction d : k4Neighborhood) {
		EdgeState state = GetEdgeSafe(loop_pos + d);
		if (state == kEdgeLine) ++n_lines;
		else if (state == kEdgeBlank) ++n_blanks;
	}

	AppliedMethod method =
		clue == 0 ? kAdjacentLines0 :
		clue == 1 ? kAdjacentLines1 :
		clue == 2 ? kAdjacentLines2 : kAdjacentLines3;

	if (n_lines == clue) {
		Move m(method);
		for (Direction d : k4Neighborhood) {
			if (GetEdgeSafe(loop_pos + d) == kEdgeUndecided) {
				m.AddTarget(loop_pos + d, kEdgeBlank);
			}
		}
		move_candidates_.push_back(m);
	}
	if (4 - n_blanks == clue) {
		Move m(method);
		for (Direction d : k4Neighborhood) {
			if (GetEdgeSafe(loop_pos + d) == kEdgeUndecided) {
				m.AddTarget(loop_pos + d, kEdgeLine);
			}
		}
		move_candidates_.push_back(m);
	}
	return n_lines == clue || 4 - n_blanks == clue;
}
void Evaluator::CheckCornerCell(CellPosition pos)
{
	if (field_.GetClue(pos) == kNoClue) return;

	Clue clue = field_.GetClue(pos);
	LoopPosition loop_pos(pos.y * 2 + 1, pos.x * 2 + 1);

	for (int i = 0; i < 4; ++i) {
		Direction d1 = k4Neighborhood[i], d2 = k4Neighborhood[(i + 1) % 4];

		if (GetEdgeSafe(loop_pos + d1 * 2 + d2) == kEdgeBlank && GetEdgeSafe(loop_pos + d2 * 2 + d1) == kEdgeBlank) {
			if (clue == 1) {
				Move m(kCornerClue1);
				m.AddTarget(loop_pos + d1, kEdgeBlank);
				m.AddTarget(loop_pos + d2, kEdgeBlank);
				move_candidates_.push_back(m);
			}
			if (clue == 2) {
				Move m1(kCornerClue2);
				if (GetEdgeSafe(loop_pos + d1) == kEdgeLine ||
					GetEdgeSafe(loop_pos + d2) == kEdgeLine ||
					GetEdgeSafe(loop_pos - d1) == kEdgeBlank ||
					GetEdgeSafe(loop_pos - d2) == kEdgeBlank ||
					GetEdgeSafe(loop_pos - d1 * 2 - d2) == kEdgeLine ||
					GetEdgeSafe(loop_pos - d2 * 2 - d1) == kEdgeLine) {
					m1.AddTarget(loop_pos + d1, kEdgeLine);
					m1.AddTarget(loop_pos + d2, kEdgeLine);
					m1.AddTarget(loop_pos - d1, kEdgeBlank);
					m1.AddTarget(loop_pos - d2, kEdgeBlank);
				}
				if (GetEdgeSafe(loop_pos + d1) == kEdgeBlank ||
					GetEdgeSafe(loop_pos + d2) == kEdgeBlank ||
					GetEdgeSafe(loop_pos - d1) == kEdgeLine ||
					GetEdgeSafe(loop_pos - d2) == kEdgeLine) {
					m1.AddTarget(loop_pos + d1, kEdgeBlank);
					m1.AddTarget(loop_pos + d2, kEdgeBlank);
					m1.AddTarget(loop_pos - d1, kEdgeLine);
					m1.AddTarget(loop_pos - d2, kEdgeLine);
				}
				if (GetEdgeSafe(loop_pos - d1 * 2 - d2) == kEdgeBlank ||
					GetEdgeSafe(loop_pos - d2 * 2 - d1) == kEdgeBlank) {
					m1.AddTarget(loop_pos - d1 * 2 - d2, kEdgeBlank);
					m1.AddTarget(loop_pos - d2 * 2 - d1, kEdgeBlank);
				}

				move_candidates_.push_back(m1);

				Move m2(kCornerClue2);
				for (int sgn : {-1, 1}) {
					if (GetEdgeSafe(loop_pos + sgn * (d1 * 2 - d2)) == kEdgeBlank) {
						m2.AddTarget(loop_pos + sgn * (d1 - d2 * 2), kEdgeLine);
					}
					if (GetEdgeSafe(loop_pos + sgn * (d1 * 2 - d2)) == kEdgeLine) {
						m2.AddTarget(loop_pos + sgn * (d1 - d2 * 2), kEdgeBlank);
					}
					if (GetEdgeSafe(loop_pos + sgn * (d2 * 2 - d1)) == kEdgeBlank) {
						m2.AddTarget(loop_pos + sgn * (d2 - d1 * 2), kEdgeLine);
					}
					if (GetEdgeSafe(loop_pos + sgn * (d2 * 2 - d1)) == kEdgeLine) {
						m2.AddTarget(loop_pos + sgn * (d2 - d1 * 2), kEdgeBlank);
					}
				}
				move_candidates_.push_back(m2);
			}
			if (clue == 3) {
				Move m(kCornerClue3);
				m.AddTarget(loop_pos + d1, kEdgeLine);
				m.AddTarget(loop_pos + d2, kEdgeLine);
				move_candidates_.push_back(m);
			}
		}
	}
}
void Evaluator::CheckLineToClue(CellPosition pos)
{
	if (field_.GetClue(pos) == kNoClue) return;

	Clue clue = field_.GetClue(pos);
	LoopPosition loop_pos(pos.y * 2 + 1, pos.x * 2 + 1);

	for (int i = 0; i < 4; ++i) {
		Direction d1 = k4Neighborhood[i], d2 = k4Neighborhood[(i + 1) % 4];
		LoopPosition in1 = loop_pos + d1 * 2 + d2, in2 = loop_pos + d2 * 2 + d1;

		if (clue == 1) {
			if ((GetEdgeSafe(in1) == kEdgeLine && GetEdgeSafe(in2) == kEdgeBlank) ||
				(GetEdgeSafe(in1) == kEdgeBlank && GetEdgeSafe(in2) == kEdgeLine)) {
				Move m(kLineToClue1);
				m.AddTarget(loop_pos - d1, kEdgeBlank);
				m.AddTarget(loop_pos - d2, kEdgeBlank);
				move_candidates_.push_back(m);
			}
		}
		if (clue == 2) {
			bool is_applicable = false;

			if (GetEdgeSafe(in1) == kEdgeLine && GetEdgeSafe(in2) == kEdgeBlank) is_applicable = true;
			if (GetEdgeSafe(in2) == kEdgeLine && GetEdgeSafe(in1) == kEdgeBlank) is_applicable = true;
			if (GetEdgeSafe(loop_pos + d1) == kEdgeLine && GetEdgeSafe(loop_pos + d2) == kEdgeBlank) is_applicable = true; // ??? diff from the prev version
			if (GetEdgeSafe(loop_pos + d2) == kEdgeLine && GetEdgeSafe(loop_pos + d1) == kEdgeBlank) is_applicable = true;

			if (!is_applicable) continue;

			Move m(kLineToClue2);

			if (GetEdgeSafe(loop_pos - d1) == kEdgeLine) m.AddTarget(loop_pos - d2, kEdgeBlank);
			if (GetEdgeSafe(loop_pos - d1) == kEdgeBlank) m.AddTarget(loop_pos - d2, kEdgeLine);
			if (GetEdgeSafe(loop_pos - d2) == kEdgeLine) m.AddTarget(loop_pos - d1, kEdgeBlank);
			if (GetEdgeSafe(loop_pos - d2) == kEdgeBlank) m.AddTarget(loop_pos - d1, kEdgeLine);
			if (GetEdgeSafe(loop_pos - d1 * 2 - d2) == kEdgeLine) m.AddTarget(loop_pos - d2 * 2 - d1, kEdgeBlank);
			if (GetEdgeSafe(loop_pos - d1 * 2 - d2) == kEdgeBlank) m.AddTarget(loop_pos - d2 * 2 - d1, kEdgeLine);
			if (GetEdgeSafe(loop_pos - d2 * 2 - d1) == kEdgeLine) m.AddTarget(loop_pos - d1 * 2 - d2, kEdgeBlank);
			if (GetEdgeSafe(loop_pos - d2 * 2 - d1) == kEdgeBlank) m.AddTarget(loop_pos - d1 * 2 - d2, kEdgeLine);

			move_candidates_.push_back(m);
		}
		if (clue == 3) {
			if (GetEdgeSafe(in1) == kEdgeLine || GetEdgeSafe(in2) == kEdgeLine) {
				Move m(kLineToClue3);
				if (GetEdgeSafe(in1) == kEdgeLine) m.AddTarget(in2, kEdgeBlank);
				if (GetEdgeSafe(in2) == kEdgeLine) m.AddTarget(in1, kEdgeBlank);
				m.AddTarget(loop_pos - d1, kEdgeLine);
				m.AddTarget(loop_pos - d2, kEdgeLine);
				move_candidates_.push_back(m);
			}
		}
	}
}
void Evaluator::CheckAlmostLineTo2(CellPosition pos)
{
	if (field_.GetClue(pos) != 2) return;

	Clue clue = field_.GetClue(pos);
	LoopPosition loop_pos(pos.y * 2 + 1, pos.x * 2 + 1);

	for (int i = 0; i < 4; ++i) {
		Direction d1 = k4Neighborhood[i], d2 = k4Neighborhood[(i + 1) % 4];
		LoopPosition in1 = loop_pos + d1 * 2 + d2, in2 = loop_pos + d2 * 2 + d1;
		LoopPosition out1 = loop_pos - d1 * 2 - d2, out2 = loop_pos - d2 * 2 - d1;

		if ((GetEdgeSafe(in1) == kEdgeLine || GetEdgeSafe(in2) == kEdgeLine) &&
			(GetEdgeSafe(loop_pos - d1) == kEdgeBlank || GetEdgeSafe(loop_pos - d2) == kEdgeBlank)) {
			Move m(kAlmostLineTo2);
			if (GetEdgeSafe(in1) == kEdgeLine) m.AddTarget(in2, kEdgeBlank);
			if (GetEdgeSafe(in2) == kEdgeLine) m.AddTarget(in1, kEdgeBlank);
			if (GetEdgeSafe(loop_pos - d1) == kEdgeBlank) m.AddTarget(loop_pos - d2, kEdgeLine);
			if (GetEdgeSafe(loop_pos - d2) == kEdgeBlank) m.AddTarget(loop_pos - d1, kEdgeLine);
			move_candidates_.push_back(m);
		}
		if (i < 2 &&
			(GetEdgeSafe(in1) == kEdgeLine || GetEdgeSafe(in2) == kEdgeLine) &&
			(GetEdgeSafe(out1) == kEdgeLine || GetEdgeSafe(out2) == kEdgeLine)) {
			Move m(kAlmostLineTo2);
			if (GetEdgeSafe(in1) == kEdgeLine) m.AddTarget(in2, kEdgeBlank);
			if (GetEdgeSafe(in2) == kEdgeLine) m.AddTarget(in1, kEdgeBlank);
			if (GetEdgeSafe(out1) == kEdgeLine) m.AddTarget(out2, kEdgeBlank);
			if (GetEdgeSafe(out2) == kEdgeLine) m.AddTarget(out1, kEdgeBlank);
			move_candidates_.push_back(m);
		}
	}
}
void Evaluator::CheckLineFromClue(CellPosition pos)
{
	if (field_.GetClue(pos) == kNoClue) return;

	Clue clue = field_.GetClue(pos);
	LoopPosition loop_pos(pos.y * 2 + 1, pos.x * 2 + 1);

	for (int i = 0; i < 4; ++i) {
		Direction d1 = k4Neighborhood[i], d2 = k4Neighborhood[(i + 1) % 4];
		LoopPosition in1 = loop_pos + d1 * 2 + d2, in2 = loop_pos + d2 * 2 + d1;

		if (clue == 1 || clue == 3) {
			EdgeState state_target = (clue == 1 ? kEdgeBlank : kEdgeLine);
			if (GetEdgeSafe(loop_pos - d1) == state_target && GetEdgeSafe(loop_pos - d2) == state_target) {
				Move m(clue == 1 ? kLineFromClue1 : kLineFromClue3);

				if (GetEdgeSafe(in1) == kEdgeLine) m.AddTarget(in2, kEdgeBlank);
				if (GetEdgeSafe(in1) == kEdgeBlank) m.AddTarget(in2, kEdgeLine);
				if (GetEdgeSafe(in2) == kEdgeLine) m.AddTarget(in1, kEdgeBlank);
				if (GetEdgeSafe(in2) == kEdgeBlank) m.AddTarget(in1, kEdgeLine);

				move_candidates_.push_back(m);
			}
		}
	}
}
void Evaluator::CheckDiagonalChain(CellPosition pos)
{
	for (int i = 0; i < 4; ++i) {
		Direction d = k4Neighborhood[i] + k4Neighborhood[(i + 1) % 4];
		Direction dy(d.y, X(0)), dx(Y(0), d.x);
		LoopPosition loop_pos(pos.y * 2 + 1, pos.x * 2 + 1);
		CellPosition cell_pos = pos;

		int cnt = -1;
		if (GetEdgeSafe(loop_pos + dy) != kEdgeUndecided && GetEdgeSafe(loop_pos + dx) != kEdgeUndecided) {
			cnt = ((GetEdgeSafe(loop_pos + dy) == kEdgeLine ? 1 : 0) + (GetEdgeSafe(loop_pos + dx) == kEdgeLine ? 1 : 0)) % 2;
		}
		if (GetEdgeSafe(loop_pos - dy) != kEdgeUndecided && GetEdgeSafe(loop_pos - dx) != kEdgeUndecided && field_.GetClue(cell_pos) != kNoClue) {
			cnt = ((GetEdgeSafe(loop_pos - dy) == kEdgeLine ? 1 : 0) + (GetEdgeSafe(loop_pos - dx) == kEdgeLine ? 1 : 0) + static_cast<int>(field_.GetClue(cell_pos))) % 2;
		}
		if (GetEdgeSafe(loop_pos - d - dy) != kEdgeUndecided && GetEdgeSafe(loop_pos - d - dx) != kEdgeUndecided && field_.GetClue(cell_pos) != kNoClue) {
			cnt = ((GetEdgeSafe(loop_pos - d - dy) == kEdgeLine ? 1 : 0) + (GetEdgeSafe(loop_pos - d - dx) == kEdgeLine ? 1 : 0) + static_cast<int>(field_.GetClue(cell_pos))) % 2;
		}
		if (cnt == -1) continue;

		for (;;) {
			loop_pos = loop_pos + 2 * d;
			cell_pos = cell_pos + d;
			if (!(0 <= loop_pos.y && loop_pos.y <= 2 * height() && 0 <= loop_pos.x && loop_pos.x <= 2 * width())) break;
			Clue clue = field_.GetClue(cell_pos);

			if (clue == 2) {
				Move m(kDiagonalChain);
				if (GetEdgeSafe(loop_pos + dy) != kEdgeUndecided) {
					m.AddTarget(loop_pos + dx, ((GetEdgeSafe(loop_pos + dy) == kEdgeLine) ^ (cnt == 1)) ? kEdgeLine : kEdgeBlank);
				}
				if (GetEdgeSafe(loop_pos + dx) != kEdgeUndecided) {
					m.AddTarget(loop_pos + dy, ((GetEdgeSafe(loop_pos + dx) == kEdgeLine) ^ (cnt == 1)) ? kEdgeLine : kEdgeBlank);
				}
				if (GetEdgeSafe(loop_pos + d + dy) != kEdgeUndecided) {
					m.AddTarget(loop_pos + d + dx, ((GetEdgeSafe(loop_pos + d + dy) == kEdgeLine) ^ (cnt == 1)) ? kEdgeLine : kEdgeBlank);
				}
				if (GetEdgeSafe(loop_pos + d + dx) != kEdgeUndecided) {
					m.AddTarget(loop_pos + d + dy, ((GetEdgeSafe(loop_pos + d + dx) == kEdgeLine) ^ (cnt == 1)) ? kEdgeLine : kEdgeBlank);
				}
				if (m.target_pos.size() > 0) {
					move_candidates_.push_back(m);
					break;
				}
				continue;
			}
			if (clue == 1 || clue == 3) {
				Move m(kDiagonalChain);
				EdgeState state = (clue == 1 ? kEdgeBlank : kEdgeLine);
				if (cnt == 0) {
					m.AddTarget(loop_pos - dy, state);
					m.AddTarget(loop_pos - dx, state);
				} else if (cnt == 1) {
					m.AddTarget(loop_pos + dy, state);
					m.AddTarget(loop_pos + dx, state);
				}
				move_candidates_.push_back(m);
			}
			break;
		}
	}
}
void Evaluator::CheckInOutRule()
{
	int number_of_cells = static_cast<int>(height()) * static_cast<int>(width());
	int out_of_grid = number_of_cells;
	UnionFind uf(2 * number_of_cells + 2);

	auto cell_id = [this](Y y, X x) {
		return static_cast<int>(y) * static_cast<int>(this->width()) + static_cast<int>(x);
	};

	for (Y y(0); y <= 2 * height(); ++y) {
		for (X x(0); x <= 2 * width(); ++x) {
			int cell1, cell2;

			if (y % 2 == 1 && x % 2 == 0) {
				cell1 = (x != 0 ? cell_id(y / 2, x / 2 - 1) : out_of_grid);
				cell2 = (x != 2 * width() ? cell_id(y / 2, x / 2) : out_of_grid);
			} else if (y % 2 == 0 && x % 2 == 1) {
				cell1 = (y != 0 ? cell_id(y / 2 - 1, x / 2) : out_of_grid);
				cell2 = (y != 2 * height() ? cell_id(y / 2, x / 2) : out_of_grid);
			} else continue;

			EdgeState state = GetEdgeSafe(LoopPosition(y, x));
			if (state == kEdgeLine) {
				uf.Join(cell1 * 2, cell2 * 2 + 1);
				uf.Join(cell1 * 2 + 1, cell2 * 2);
			} else if (state == kEdgeBlank) {
				uf.Join(cell1 * 2, cell2 * 2);
				uf.Join(cell1 * 2 + 1, cell2 * 2 + 1);
			}
		}
	}
	for (Y y(0); y <= 2 * height(); ++y) {
		for (X x(0); x <= 2 * width(); ++x) {
			int cell1, cell2;

			if (y % 2 == 1 && x % 2 == 0) {
				cell1 = (x != 0 ? cell_id(y / 2, x / 2 - 1) : out_of_grid);
				cell2 = (x != 2 * width() ? cell_id(y / 2, x / 2) : out_of_grid);
			} else if (y % 2 == 0 && x % 2 == 1) {
				cell1 = (y != 0 ? cell_id(y / 2 - 1, x / 2) : out_of_grid);
				cell2 = (y != 2 * height() ? cell_id(y / 2, x / 2) : out_of_grid);
			} else continue;

			if (uf.Root(cell1 * 2) == uf.Root(cell2 * 2)) {
				// TODO: avoid duplicated addition
				move_candidates_.push_back(Move(LoopPosition(y, x), kEdgeBlank, kInoutRule));
			}
			if (uf.Root(cell1 * 2) == uf.Root(cell2 * 2 + 1)) {
				move_candidates_.push_back(Move(LoopPosition(y, x), kEdgeLine, kInoutRule));
			}
		}
	}
}
}
}
