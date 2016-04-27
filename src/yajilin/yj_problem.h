#pragma once

#include "../common/grid.h"

namespace penciloid
{
namespace yajilin
{
class Problem
{
public:
	enum ClueDirection
	{
		kNoClue,
		kClueNorth, // The direction in which y coordinate is decreasing
		kClueWest, // The direction in which x coordinate is decreasing
		kClueEast, // The direction in which x coordinate is increasing
		kClueSouth // The direction in which y coordinate is increasing
	};

	struct Cell
	{
		ClueDirection direction;
		int clue_number;

		Cell() : direction(kNoClue), clue_number(0) {}
		Cell(ClueDirection direction, int clue_number) : direction(direction), clue_number(clue_number) {}
	};

	Problem();
	Problem(Y height, X width);
	Problem(const Problem &other);
	Problem(Problem &&other);

	~Problem();

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }
	
	void SetClue(CellPosition pos, Cell value) { cells_.at(pos) = value; }
	void SetClue(CellPosition pos, ClueDirection dir, int clue_number) { SetClue(pos, Cell(dir, clue_number)); }
	Cell GetClue(CellPosition pos) const { return cells_.at(pos); }

private:
	Grid<Cell> cells_;
};
}
}