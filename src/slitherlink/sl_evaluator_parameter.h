#pragma once

namespace penciloid
{
namespace slitherlink
{
struct EvaluatorParameter
{
public:
	static const int kNumberOfEffectiveParameters = 25;
	
	EvaluatorParameter();
	double &operator[](int i) { return *(at(i)); }

	double two_lines;
	double avoid_cycle, eliminate_closed_chain, hourglass_rule;

	double adjacent_lines[4];
	double adjacent_3, diagonal_3, diagonal_3_avoid_cycle;
	double corner_clue[4];
	double corner_clue_2_hard;
	double line_to_clue[4];
	double line_from_clue[4];
	double almost_line_to_2;
	double diagonal_chain;
	double inout_rule;

	double locality_base;
	double alternative_dimension;
	double undecided_power;

private:
	double *at(int i);
};
}
}
