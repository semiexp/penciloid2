#pragma once

#include <vector>

#include "sl_problem.h"
#include "sl_field.h"
#include "sl_evaluator_parameter.h"
#include "sl_evaluator_detailed_result.h"

namespace penciloid
{
namespace slitherlink
{
class Evaluator
{
public:
	enum AppliedMethod
	{
		kTwoLines,
		kAvoidCycle,
		kEliminateClosedChain,
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
		kCornerClue2Hard,
		kLineToClue1,
		kLineToClue2,
		kLineToClue3,
		kLineFromClue1,
		kLineFromClue3,
		kAlmostLineTo2,
		kDiagonalChain,
		kInoutRule
	};

	static const double kScoreImpossible;
	static const double kScoreInconsistent;

	Evaluator();
	Evaluator(Problem &problem);
	Evaluator(const Evaluator&) = delete;
	Evaluator(Evaluator&&) = delete;
	~Evaluator();

	Evaluator &operator=(const Evaluator&) = delete;
	Evaluator &operator=(Evaluator&&) = delete;

	void SetParameter(const EvaluatorParameter &param) {
		param_given_ = param;
		param_ = param;
		for (int i = 0; i < EvaluatorParameter::kNumberOfEffectiveParameters; ++i) param_[i] -= i * 1e-7;
	}
	EvaluatorParameter GetParameter() const { return param_given_; }

	double Evaluate();
	EvaluatorDetailedResult GetDetailedResult() const { return result_; }

private:
	typedef Field::EdgeState EdgeState;
	static const EdgeState kEdgeUndecided = Field::kEdgeUndecided;
	static const EdgeState kEdgeLine = Field::kEdgeLine;
	static const EdgeState kEdgeBlank = Field::kEdgeBlank;

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
	double GetScoreOfMove(Move &move);

	void CheckAvoidCycleRule();
	void CheckClosedChain();
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
	EvaluatorParameter param_, param_given_;
	std::vector<Move> move_candidates_;

	EvaluatorDetailedResult result_;
};
}
}
