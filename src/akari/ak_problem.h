#pragma once

#include "../common/type.h"
#include "../common/grid.h"
#include "ak_type.h"

namespace penciloid
{
namespace akari
{
class Problem
{
public:
	Problem();

	// Initialize a problem with the empty board
	Problem(Y height, X width);

	// Place clues according to <clues>.
	// '0', '1', '2', '3' and '4' stand for a block cell with a clue.
	// '#' stands for a block cell with no clue.
	// Other characters stand for empty cells.
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
