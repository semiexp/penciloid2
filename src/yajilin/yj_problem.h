#pragma once

#include "../common/grid.h"
#include "yj_type.h"

namespace penciloid
{
namespace yajilin
{
class Problem
{
public:
	Problem();
	Problem(Y height, X width);
	Problem(const Problem &other);
	Problem(Problem &&other);

	~Problem();

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }
	
	void SetClue(CellPosition pos, Clue value) { cells_.at(pos) = value; }
	void SetClue(CellPosition pos, ClueDirection dir, int clue_number) { SetClue(pos, Clue(dir, clue_number)); }
	Clue GetClue(CellPosition pos) const { return cells_.at(pos); }

private:
	Grid<Clue> cells_;
};
}
}