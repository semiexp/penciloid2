#include "sl_evaluator_parameter.h"

namespace penciloid
{
namespace slitherlink
{
EvaluatorParameter::EvaluatorParameter()
{
	two_lines = 0.0;
	avoid_cycle = 1.05;
	hourglass_rule = 1.7;

	adjacent_lines[0] = 1.0;
	adjacent_lines[1] = 0.85;
	adjacent_lines[2] = 1.25;
	adjacent_lines[3] = 1.9;
	adjacent_3 = 4.85;
	diagonal_3 = 2.3;
	diagonal_3_avoid_cycle = 2.0;

	corner_clue[0] = 0.0;
	corner_clue[1] = 2.45;
	corner_clue[2] = 3.35;
	corner_clue[3] = 1.1;
	corner_clue_2_hard = 4.9;
	line_to_clue[0] = 0.0;
	line_to_clue[1] = 2.0;
	line_to_clue[2] = 0.85;
	line_to_clue[3] = 2.05;
	line_from_clue[0] = 0.0;
	line_from_clue[1] = 4.05;
	line_from_clue[2] = 0.0;
	line_from_clue[3] = 2.05;
	almost_line_to_2 = 2.55;
	diagonal_chain = 2.35;
	inout_rule = 3.75;

	locality_base = 2.2;
	alternative_dimension = 3.9;
	undecided_power = 0.35;
}
double* EvaluatorParameter::at(int i)
{
	switch (i)
	{
	case 0: return &avoid_cycle;
	case 1: return &hourglass_rule;
	case 2: return &(adjacent_lines[0]);
	case 3: return &(adjacent_lines[1]);
	case 4: return &(adjacent_lines[2]);
	case 5: return &(adjacent_lines[3]);
	case 6: return &adjacent_3;
	case 7: return &diagonal_3;
	case 8: return &diagonal_3_avoid_cycle;
	case 9: return &(corner_clue[1]);
	case 10: return &(corner_clue[2]);
	case 11: return &(corner_clue[3]);
	case 12: return &corner_clue_2_hard;
	case 13: return &(line_to_clue[1]);
	case 14: return &(line_to_clue[2]);
	case 15: return &(line_to_clue[3]);
	case 16: return &(line_from_clue[1]);
	case 17: return &(line_from_clue[3]);
	case 18: return &almost_line_to_2;
	case 19: return &diagonal_chain;
	case 20: return &inout_rule;
	case 21: return &locality_base;
	case 22: return &alternative_dimension;
	case 23: return &undecided_power;
	}
	return nullptr;
}
}
}
