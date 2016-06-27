#pragma once

#include "nk_type.h"
#include "nk_problem.h"
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
	Field(const Problem &problem);
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

	CellState GetCell(CellPosition pos) const { return cells_.at(pos).status; }
	Clue GetClue(CellPosition pos) const { return cells_.at(pos).clue; }

	void DecideCell(CellPosition pos, CellState status);

	void ExpandBlack();
	void ExpandWhite();

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

	int GetIndex(CellPosition pos) { return cells_.GetIndex(pos); }
	int GetRoot(int cell_idx);
	int GetGroupSize(int cell_idx) { return -cells_.at(GetRoot(cell_idx)).group_parent_cell; }
	bool HasClueInGroup(int cell_idx) { return cells_.at(GetRoot(cell_idx)).clue != kNoClue; }
	void Join(int cell_idx1, int cell_idx2);
	
	void CheckCluster(CellPosition top);
	void AvoidGroupWithSeveralClue(CellPosition pos);
	void CloseGroup(int cell_idx);

	Grid<Cell> cells_;
	bool inconsistent_, fully_solved_;
};
}
}
