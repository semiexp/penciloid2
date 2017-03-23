#pragma once

namespace penciloid
{
namespace slitherlink
{
struct DictionaryMethod
{
	DictionaryMethod() :
		two_lines(true),
		adjacent_lines(true),
		corner_clue_1(true), corner_clue_2(true), corner_clue_3(true),
		corner_clue_2_hard(true),
		line_to_clue_1(true), line_to_clue_2(true), line_to_clue_3(true),
		partial_line_to_clue_2(true),
		line_from_clue_1(true), line_from_clue_3(true)
	{}

	bool two_lines;
	bool adjacent_lines;
	bool corner_clue_1, corner_clue_2, corner_clue_3;
	bool corner_clue_2_hard;
	bool line_to_clue_1, line_to_clue_2, line_to_clue_3;
	bool partial_line_to_clue_2;
	bool line_from_clue_1, line_from_clue_3;
};
}
}