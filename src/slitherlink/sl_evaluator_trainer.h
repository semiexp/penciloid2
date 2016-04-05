#pragma once 

#include <vector>
#include <iostream>

#include "sl_evaluator.h"
#include "sl_evaluator_parameter.h"
#include "sl_problem.h"

namespace penciloid
{
namespace slitherlink
{
class EvaluatorTrainer
{
public:
	EvaluatorTrainer(EvaluatorParameter param = EvaluatorParameter()) : _param(param), _num_threads(1) {}

	void AddProblemSet(std::vector<Problem> &set) { _problem_sets.push_back(set); }
	void LoadProblemSetFromFile(std::istream &is);

	void SetNumberOfThreads(int n) { _num_threads = n; }
	int GetNumberOfThreads() const { return _num_threads; }
	void SetParameter(EvaluatorParameter param) { _param = param; }
	EvaluatorParameter GetParameter() const { return _param; }

	void Train();

private:
	double ComputeScore();

	std::vector<std::vector<Problem> > _problem_sets;
	std::vector<std::vector<bool> > _is_unsolvable;
	EvaluatorParameter _param;
	int _num_threads;
};
}
}