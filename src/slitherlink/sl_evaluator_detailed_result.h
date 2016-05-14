#pragma once

#include <vector>

namespace penciloid
{
namespace slitherlink
{
struct EvaluatorDetailedResult
{
	std::vector<int> method_application_count;
	std::vector<double> step_score;
	double score;
};
}
}
