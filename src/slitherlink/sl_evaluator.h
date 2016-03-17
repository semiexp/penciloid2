#pragma once

#include <vector>

#include "sl_problem.h"
#include "sl_field.h"
#include "sl_evaluator_parameter.h"

namespace penciloid
{
namespace slitherlink
{
class Evaluator
{
public:
	const double kScoreImpossible = -1.0;
	const double kScoreInconsistent = -2.0;

	Evaluator();
	Evaluator(Problem &problem);
	Evaluator(const Evaluator&) = delete;
	Evaluator(Evaluator&&) = delete;
	~Evaluator();

	Evaluator &operator=(const Evaluator&) = delete;
	Evaluator &operator=(Evaluator&&) = delete;

	void SetParameter(const EvaluatorParameter &param) { param_ = param; }
	EvaluatorParameter GetParameter() const { return param_; }

	double EvaluateRun();

private:
	typedef Field::EdgeState EdgeState;
	const EdgeState kEdgeUndecided = Field::kEdgeUndecided;
	const EdgeState kEdgeLine = Field::kEdgeLine;
	const EdgeState kEdgeBlank = Field::kEdgeBlank;

	struct Move
	{
		Move(double difficulty = 0.0) : difficulty(difficulty), target_pos(), target_state() {}
		Move(LoopPosition pos, EdgeState state, double difficulty) : difficulty(difficulty) {
			target_pos.push_back(pos);
			target_state.push_back(state);
		}
		void AddTarget(LoopPosition pos, EdgeState st) {
			target_pos.push_back(pos);
			target_state.push_back(st);
		}
		double difficulty;
		std::vector<LoopPosition> target_pos;
		std::vector<EdgeState> target_state;
	};

	Y height() { return field_.height(); }
	X width() { return field_.width(); }
	EdgeState GetEdgeSafe(LoopPosition pos) { return field_.GetEdgeSafe(pos); }

	void EnumerateMoves();
	void EliminateDoneMoves();

	void CheckAvoidCycleRule();
	void CheckHourglassRule(LoopPosition pos);
	void CheckTwoLinesRule();
	void CheckTheoremsAbout3();
	bool CheckAdjacentLinesRule(CellPosition pos);
	void CheckCornerCell(CellPosition pos);
	void CheckLineToClue(CellPosition pos);
	void CheckLineFromClue(CellPosition pos);
	void CheckDiagonalChain(CellPosition pos);
	void CheckInOutRule();

	Field field_;
	EvaluatorParameter param_;
	std::vector<Move> move_candidates_;
};
}
}
