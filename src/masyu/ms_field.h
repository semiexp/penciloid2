#pragma once

#include <iostream>

#include "../common/grid_loop.h"
#include "../common/grid.h"
#include "ms_problem.h"

namespace penciloid
{
namespace masyu
{
class Field : public GridLoop<Field>
{
public:
	Field();
	Field(Y height, X width);
	Field(Problem &problem);
	Field(const Field &other);
	Field(Field &&other);

	~Field();

	Field &operator=(const Field &other);
	Field &operator=(Field &&other);

	Y height() const { return clues_.height(); }
	X width() const { return clues_.width(); }

	void AddClue(CellPosition cell, Clue clue);
	Clue GetClue(CellPosition cell) { return clues_(cell); }

	void CheckNeighborhood(LoopPosition edge);
	void Inspect(LoopPosition pos);

private:
	void ApplyTheorem(LoopPosition pos);

	Grid<Clue> clues_;
};

std::ostream& operator<<(std::ostream &stream, Field &field);

}
}
