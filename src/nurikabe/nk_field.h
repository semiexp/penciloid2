#pragma once

#include "nk_type.h"
#include "../common/type.h"
#include "../common/grid.h"

namespace penciloid
{
namespace nurikabe
{
class Field
{
public:
	enum CellState
	{
		kCellUndecided,
		kCellBlack,
		kCellWhite
	};

	Field();
	Field(Y height, X width);
	Field(const Field &other);
	Field(Field &&other);
	~Field();
	
	Field &operator=(const Field &other);
	Field &operator=(Field &&other);

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }

	bool IsInconsistent() const { return inconsistent_; }
	bool IsFullySolved() const { return fully_solved_; }
	void SetInconsistent() { inconsistent_ = true; }

	CellState GetCell(CellPosition pos) { return cells_.at(pos).status; }
	Clue GetClue(CellPosition pos) { return cells_.at(pos).clue; }

	void DecideCell(CellPosition pos, CellState status);

private:
	struct Cell
	{
		// Values of group_parent_cell form a Union-Find structure.
		// If group_parent_cell < 0, (-group_parent_cell) is equal to the size of the group.
		// Values of group_next_cell form a circular-list structure; following these values leads to every cell in the same group.
		// If a cell <c> contains a clue, it is a root in the Union-Find structure.

		// group_next_cell can't be properly initialized here; it should be updated before using it.
		Cell(Clue clue = kNoClue, CellState status = kCellUndecided) :
			clue(clue), status(status), group_parent_cell(-1), group_next_cell(-1) {}

		Clue clue;
		CellState status;
		int group_parent_cell;
		int group_next_cell;
	};

	int GetRoot(int cell_idx);
	void Join(int cell_idx1, int cell_idx2);

	Grid<Cell> cells_;
	bool inconsistent_, fully_solved_;
};
}
}