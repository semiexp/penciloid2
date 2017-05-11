#pragma once

#include "../common/type.h"
#include "../common/grid.h"
#include "nl_type.h"

namespace penciloid
{
namespace numberlink
{
class Problem
{
public:
	Problem();

	// Initialize a problem with the empty board
	Problem(Y height, X width);

	// Place clues according to <clues>. Only '0', '1', '2' and '3' are considered as clues; other characters stand for empty cells.
	Problem(Y height, X width, const char* clues[]);

	~Problem() {}

	inline Y height() const { return grid_.height(); }
	inline X width() const { return grid_.width(); }
	inline void SetClue(CellPosition cell, Clue clue) { grid_(cell) = clue; }
	inline Clue GetClue(CellPosition cell) const { return grid_(cell); }

private:
	Grid<Clue> grid_;
};
}
}
