#pragma once

#include "../common/grid_loop.h"
#include "../common/grid.h"
#include "yj_problem.h"

namespace penciloid
{
namespace yajilin
{
class Field : public GridLoop<Field>
{
public:
	enum CellState
	{
		kCellUndecided,
		kCellLine,
		kCellBlock,
		kCellClue
	};
	enum ClueDirection
	{
		kClueNorth, // The direction in which y coordinate is decreasing
		kClueWest, // The direction in which x coordinate is decreasing
		kClueEast, // The direction in which x coordinate is increasing
		kClueSouth // The direction in which y coordinate is increasing
	};
	struct Cell
	{
		CellState status;
		ClueDirection direction;
		int clue_number;

		Cell() : status(kCellUndecided), direction(kClueNorth), clue_number(-1) {}
		Cell(CellState status, ClueDirection direction, int clue_number) : status(status), direction(direction), clue_number(clue_number) {}
	};

	Field();
	Field(Y height, X width);
	Field(Problem &problem);
	Field(const Field &other);
	Field(Field &&other);

	~Field();

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }

	void DecideCell(CellPosition cell, CellState status);
	CellState GetCellState(CellPosition cell) { return cells_.at(cell).status; }

	void Inspect(LoopPosition pos);
	void HasDecided(LoopPosition pos);

private:
	Grid<Cell> cells_;

	Direction GetDirectionValue(ClueDirection dir);
};
}
}
