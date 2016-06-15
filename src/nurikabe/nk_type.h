#pragma once

namespace penciloid
{
namespace nurikabe
{
struct Clue
{
	Clue() : clue_low(-1), clue_high(-1) {}
	Clue(int c) : clue_low(c), clue_high(c) {}
	Clue(int low, int high) : clue_low(low), clue_high(high) {}

	int clue_low, clue_high;
};

inline bool operator==(const Clue &lhs, const Clue &rhs)
{
	return lhs.clue_low == rhs.clue_low && lhs.clue_high == rhs.clue_high;
}
inline bool operator!=(const Clue &lhs, const Clue &rhs)
{
	return !(lhs == rhs);
}

const Clue kNoClue(-1, -1);

}
}
