#pragma once

#include <iostream>

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
	struct Cell
	{
		CellState status;
		Clue clue;

		Cell() : status(kCellUndecided), clue() {}
		Cell(CellState status, Clue clue) : status(status), clue(clue) {}
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
	Cell GetCell(CellPosition cell) { return cells_.at(cell); }

	void Inspect(LoopPosition pos);
	void HasDecided(LoopPosition pos);

private:
	Grid<Cell> cells_;

	Direction GetDirectionValue(ClueDirection dir);
};

std::ostream& operator<<(std::ostream &stream, Field &field);

}
}
