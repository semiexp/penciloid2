#pragma once

#include "../common/type.h"
#include "../common/grid.h"

#include <iostream>

#include "ak_problem.h"

namespace penciloid
{
namespace akari
{
class Field
{
public:
	enum CellState {
		CELL_UNDECIDED,
		CELL_LIGHT,
		CELL_NO_LIGHT_NOT_LIT,
		CELL_LIT_BY_OTHER,
		CELL_BLOCK,
		CELL_NO_LIGHT // only for DecideCell
	};

	Field();
	Field(const Problem &problem);

	~Field();

	Y height() const { return cells_.height(); }
	X width() const { return cells_.width(); }

	bool IsInconsistent() const { return inconsistent_; }
	void SetInconsistent() { inconsistent_ = true; }

	CellState GetCell(CellPosition pos) const;
	CellState GetCellSafe(CellPosition pos) const;
	Clue GetClueValue(CellPosition pos) const;
	void DecideCell(CellPosition pos, CellState status);
private:
	enum GroupDirection {
		GROUP_HORIZONTAL,
		GROUP_VERTICAL
	};
	struct Cell
	{
		CellState status;
		Clue clue_number;
		int group_horizontal_id, group_vertical_id;
	};
	
	// maintains each group of consecutive cells
	struct CellGroup
	{
		int cell_count;
		int xor_remaining_cell_id;
		CellPosition group_top;
		GroupDirection dir;
	};

	void CheckCell(CellPosition pos);
	void CheckNeighbor(CellPosition pos);
	void CheckGroup(int group_id);
	void ExcludeCellFromGroups(CellPosition pos);

	Grid<Cell> cells_;
	CellGroup *groups_;
	bool inconsistent_;
};

std::ostream& operator<<(std::ostream &stream, Field &field);

}
}
