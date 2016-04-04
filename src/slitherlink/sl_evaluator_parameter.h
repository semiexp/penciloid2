#pragma once

namespace penciloid
{
namespace slitherlink
{
struct EvaluatorParameter
{
public:
	static const int kNumberOfEffectiveParameters = 22;
	
	EvaluatorParameter();
	double &operator[](int i) { *(at(i)); }

	double two_lines;
	double avoid_cycle, hourglass_rule;

	double adjacent_lines[4];
	double adjacent_3, diagonal_3, diagonal_3_avoid_cycle;
	double corner_clue[4];
	double line_to_clue[4];
	double line_from_clue[4];
	double almost_line_to_2;
	double diagonal_chain;
	double inout_rule;

	double locality_base;
	double alternative_dimension;

private:
	double *at(int i);
};
}
}
