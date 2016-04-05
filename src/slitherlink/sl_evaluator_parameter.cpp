#include "sl_evaluator_parameter.h"

namespace penciloid
{
namespace slitherlink
{
EvaluatorParameter::EvaluatorParameter()
{
	two_lines = 0.0;
	avoid_cycle = 1.2;
	hourglass_rule = 3.1;

	adjacent_lines[0] = 1.3;
	adjacent_lines[1] = 1.4;
	adjacent_lines[2] = 1.2;
	adjacent_lines[3] = 1.0;
	adjacent_3 = 2.5;
	diagonal_3 = 3.3;
	diagonal_3_avoid_cycle = 1.6;

	corner_clue[0] = 0.0;
	corner_clue[1] = 3.1;
	corner_clue[2] = 4.0;
	corner_clue[3] = 1.3;
	line_to_clue[0] = 0.0;
	line_to_clue[1] = 2.4;
	line_to_clue[2] = 1.9;
	line_to_clue[3] = 1.4;
	line_from_clue[0] = 0.0;
	line_from_clue[1] = 4.9;
	line_from_clue[2] = 0.0;
	line_from_clue[3] = 3.9;
	almost_line_to_2 = 3.3;
	diagonal_chain = 3.0;
	inout_rule = 4.8;

	locality_base = 2.3;
	alternative_dimension = 2.9;
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
	case 12: return &(line_to_clue[1]);
	case 13: return &(line_to_clue[2]);
	case 14: return &(line_to_clue[3]);
	case 15: return &(line_from_clue[1]);
	case 16: return &(line_from_clue[3]);
	case 17: return &almost_line_to_2;
	case 18: return &diagonal_chain;
	case 19: return &inout_rule;
	case 20: return &locality_base;
	case 21: return &alternative_dimension;
	}
	return nullptr;
}
}
}
