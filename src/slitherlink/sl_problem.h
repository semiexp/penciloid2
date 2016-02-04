#pragma once

#include "../common/type.h"
#include "../common/grid.h"
#include "sl_type.h"

namespace penciloid
{
namespace slitherlink
{
class Problem : public Grid<Clue>
{
public:
	Problem();

	// Initialize a problem with the empty board
	Problem(Y height, X width);

	// Place clues according to <clues>. Only '0', '1', '2' and '3' are considered as clues; other characters stand for empty cells.
	Problem(Y height, X width, const char* clues[]);

	~Problem() {}

	inline void SetClue(Position cell, Clue clue) { at(cell) = clue; }
	inline Clue GetClue(Position cell) const { return at(cell); }
};
}
}
