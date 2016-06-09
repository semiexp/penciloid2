#pragma once

#include "../common/grid.h"
#include "ms_type.h"

namespace penciloid
{
namespace masyu
{
class Problem
{
public:
	Problem();
	Problem(Y height, X width);
	Problem(const Problem &other);
	Problem(Problem &&other);

	~Problem();

	Problem &operator=(const Problem &other);
	Problem &operator=(Problem &&other);

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }

	void SetClue(CellPosition pos, Clue value) { cells_.at(pos) = value; }
	Clue GetClue(CellPosition pos) const { return cells_.at(pos); }

private:
	Grid<Clue> cells_;
};
}
}
