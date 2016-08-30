#pragma once

namespace penciloid
{
namespace kakuro
{
struct Clue
{
	Clue() : horizontal(-1), vertical(-1) {}
	Clue(int horizontal, int vertical) : horizontal(horizontal), vertical(vertical) {}

	int horizontal, vertical;
};

inline bool operator==(const Clue &lhs, const Clue &rhs)
{
	return lhs.horizontal == rhs.horizontal && lhs.vertical == rhs.vertical;
}
inline bool operator!=(const Clue &lhs, const Clue &rhs)
{
	return !(lhs == rhs);
}

const Clue kEmptyCell(-2, -2);
const int kNoClueValue = -1;
const Clue kNoClueCell(kNoClueValue, kNoClueValue);

}
}