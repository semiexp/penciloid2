#pragma once 

#include <vector>

#include "sl_evaluator.h"
#include "sl_evaluator_parameter.h"
#include "sl_problem.h"

namespace penciloid
{
namespace slitherlink
{
class EvaluatorTrainingSet
{
public:
	EvaluatorTrainingSet() : problem_set_(), evaluability_() {}

	void AddProblem(Problem &problem) {
		problem_set_.push_back(problem);
		evaluability_.push_back(kUndecided);
	}
	Problem operator[](int i) const { return problem_set_[i]; }

	std::vector<double> ComputeDifficultyAll(EvaluatorParameter param, int n_threads = 1);

private:
	enum Evaluability {
		kUndecided, kEvaluable, kUnevaluable
	};
	std::vector<Problem> problem_set_;
	std::vector<Evaluability> evaluability_;
};
}
}