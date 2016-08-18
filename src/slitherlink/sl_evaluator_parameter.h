#pragma once

namespace penciloid
{
namespace slitherlink
{
struct EvaluatorParameter
{
public:
	static const int kNumberOfEffectiveParameters = 26;
	
	static const int kIndexAvoidCycle = 0;
	static const int kIndexEliminateClosedChain = 1;
	static const int kIndexHourglassRule = 2;
	static const int kIndexAdjacentLines0 = 3;
	static const int kIndexAdjacentLines1 = 4;
	static const int kIndexAdjacentLines2 = 5;
	static const int kIndexAdjacentLines3 = 6;
	static const int kIndexAdjacent3 = 7;
	static const int kIndexDiagonal3 = 8;
	static const int kIndexDiagonal3AvoidCycle = 9;
	static const int kIndexCornerClue1 = 10;
	static const int kIndexCornerClue2 = 11;
	static const int kIndexCornerClue3 = 12;
	static const int kIndexCornerClue2Hard = 13;
	static const int kIndexLineToClue1 = 14;
	static const int kIndexLineToClue2 = 15;
	static const int kIndexLineToClue3 = 16;
	static const int kIndexLineFromClue1 = 17;
	static const int kIndexLineFromClue3 = 18;
	static const int kIndexAlmostLineTo2 = 19;
	static const int kIndexDiagonalChain = 20;
	static const int kIndexInoutRule = 21;
	static const int kIndexLocalityBase = 22;
	static const int kIndexAlternativeDimension = 23;
	static const int kIndexUndecidedPower = 24;
	static const int kIndexLocalityDistance = 25;

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

	double locality_distance;

private:
	double *at(int i);
};
}
}
