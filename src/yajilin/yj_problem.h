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

	// Initialize a problem with the empty board
	Problem(Y height, X width);

	// Place clues according to <clues>.
	// Each cell is represented by 2 characters in <clues>.
	// The first character of a cell stands for the direction of the clue.
	//   ^: up
	//   <: left
	//   >: right
	//   v: down
	//   Other characters stand for the cell without a clue.
	// The second character of a cell stands for the number of the clue (numbers between 0 and 9 can be represented).
	Problem(Y height, X width, const char* clues[]);

	Problem(const Problem &other);
	Problem(Problem &&other);

	~Problem();

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }
	
	void SetClue(CellPosition pos, Clue value) { cells_(pos) = value; }
	void SetClue(CellPosition pos, ClueDirection dir, int clue_number) { SetClue(pos, Clue(dir, clue_number)); }
	Clue GetClue(CellPosition pos) const { return cells_(pos); }

private:
	Grid<Clue> cells_;
};
}
}