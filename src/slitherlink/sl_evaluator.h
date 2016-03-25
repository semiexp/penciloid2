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
	static const double kScoreImpossible;
	static const double kScoreInconsistent;

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
	static const EdgeState kEdgeUndecided = Field::kEdgeUndecided;
	static const EdgeState kEdgeLine = Field::kEdgeLine;
	static const EdgeState kEdgeBlank = Field::kEdgeBlank;

	enum AppliedMethod
	{
		kTwoLines,
		kAvoidCycle,
		kHourglassRule,
		kAdjacentLines0,
		kAdjacentLines1,
		kAdjacentLines2,
		kAdjacentLines3,
		kAdjacent3s,
		kDiagonal3s,
		kDiagonal3sAvoidCycle,
		kCornerClue1,
		kCornerClue2,
		kCornerClue3,
		kLineToClue1,
		kLineToClue2,
		kLineToClue3,
		kLineFromClue1,
		kLineFromClue3,
		kAlmostLineTo2,
		kDiagonalChain,
		kInoutRule
	};

	struct Move
	{
		Move(AppliedMethod method) : method(method), target_pos(), target_state() {}
		Move(LoopPosition pos, EdgeState state, AppliedMethod method) : method(method) {
			target_pos.push_back(pos);
			target_state.push_back(state);
		}
		void AddTarget(LoopPosition pos, EdgeState st) {
			target_pos.push_back(pos);
			target_state.push_back(st);
		}
		AppliedMethod method;
		std::vector<LoopPosition> target_pos;
		std::vector<EdgeState> target_state;
	};

	struct ScoredMove
	{
		ScoredMove(double score, Move move) : score(score), move(move) {}
		double score;
		Move move;
	};

	Y height() { return field_.height(); }
	X width() { return field_.width(); }
	EdgeState GetEdgeSafe(LoopPosition pos) { return field_.GetEdgeSafe(pos); }

	void EnumerateMoves();
	void EliminateDoneMoves();
	double GetScoreOfMethod(AppliedMethod method);

	void CheckAvoidCycleRule();
	void CheckHourglassRule(LoopPosition pos);
	void CheckTwoLinesRule();
	void CheckTheoremsAbout3();
	bool CheckAdjacentLinesRule(CellPosition pos);
	void CheckCornerCell(CellPosition pos);
	void CheckLineToClue(CellPosition pos);
	void CheckAlmostLineTo2(CellPosition pos);
	void CheckLineFromClue(CellPosition pos);
	void CheckDiagonalChain(CellPosition pos);
	void CheckInOutRule();

	Field field_;
	EvaluatorParameter param_;
	std::vector<Move> move_candidates_;
};
}
}
