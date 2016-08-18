#include "sl_evaluator_parameter.h"

namespace penciloid
{
namespace slitherlink
{
EvaluatorParameter::EvaluatorParameter()
{
	two_lines = 0.0;
	avoid_cycle = 1.05;
	eliminate_closed_chain = 1.0;
	hourglass_rule = 1.7;

	adjacent_lines[0] = 1.0;
	adjacent_lines[1] = 0.85;
	adjacent_lines[2] = 1.25;
	adjacent_lines[3] = 1.9;
	adjacent_3 = 4.95;
	diagonal_3 = 2.3;
	diagonal_3_avoid_cycle = 1.95;

	corner_clue[0] = 0.0;
	corner_clue[1] = 2.45;
	corner_clue[2] = 3.3;
	corner_clue[3] = 1.1;
	corner_clue_2_hard = 4.7;
	line_to_clue[0] = 0.0;
	line_to_clue[1] = 1.95;
	line_to_clue[2] = 0.85;
	line_to_clue[3] = 2.1;
	line_from_clue[0] = 0.0;
	line_from_clue[1] = 4.05;
	line_from_clue[2] = 0.0;
	line_from_clue[3] = 2.05;
	almost_line_to_2 = 2.8;
	diagonal_chain = 2.35;
	inout_rule = 4.25;

	locality_base = 2.3;
	alternative_dimension = 3.95;
	undecided_power = 0.35;

	locality_distance = 4.0;
}
double* EvaluatorParameter::at(int i)
{
	switch (i)
	{
	case kIndexAvoidCycle: return &avoid_cycle;
	case kIndexEliminateClosedChain: return &eliminate_closed_chain;
	case kIndexHourglassRule: return &hourglass_rule;
	case kIndexAdjacentLines0: return &(adjacent_lines[0]);
	case kIndexAdjacentLines1: return &(adjacent_lines[1]);
	case kIndexAdjacentLines2: return &(adjacent_lines[2]);
	case kIndexAdjacentLines3: return &(adjacent_lines[3]);
	case kIndexAdjacent3: return &adjacent_3;
	case kIndexDiagonal3: return &diagonal_3;
	case kIndexDiagonal3AvoidCycle: return &diagonal_3_avoid_cycle;
	case kIndexCornerClue1: return &(corner_clue[1]);
	case kIndexCornerClue2: return &(corner_clue[2]);
	case kIndexCornerClue3: return &(corner_clue[3]);
	case kIndexCornerClue2Hard: return &corner_clue_2_hard;
	case kIndexLineToClue1: return &(line_to_clue[1]);
	case kIndexLineToClue2: return &(line_to_clue[2]);
	case kIndexLineToClue3: return &(line_to_clue[3]);
	case kIndexLineFromClue1: return &(line_from_clue[1]);
	case kIndexLineFromClue3: return &(line_from_clue[3]);
	case kIndexAlmostLineTo2: return &almost_line_to_2;
	case kIndexDiagonalChain: return &diagonal_chain;
	case kIndexInoutRule: return &inout_rule;
	case kIndexLocalityBase: return &locality_base;
	case kIndexAlternativeDimension: return &alternative_dimension;
	case kIndexUndecidedPower: return &undecided_power;
	case kIndexLocalityDistance: return &locality_distance;
	}
	return nullptr;
}
}
}
