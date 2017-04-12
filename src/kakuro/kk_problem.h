#pragma once

#include "kk_type.h"
#include "../common/grid.h"

namespace penciloid
{
namespace kakuro
{
class Problem
{
public:
	Problem();

	// Initialize a problem with the empty board (each cell is initialized with kEmptyCell)
	Problem(Y height, X width);
	Problem(const Problem &other);
	Problem(Problem &&other);
	~Problem() {}

	Problem &operator=(const Problem &other);
	Problem &operator=(Problem &&other);

	inline Y height() const { return grid_.height(); }
	inline X width() const { return grid_.width(); }
	inline void SetClue(CellPosition cell, Clue clue) { grid_.at(cell) = clue; }
	inline Clue GetClue(CellPosition cell) const { return grid_.at(cell); }

private:
	Grid<Clue> grid_;
};
}
}
