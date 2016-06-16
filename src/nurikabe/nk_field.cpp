#include "nk_field.h"

#include <xutility>
#include <algorithm>

namespace penciloid
{
namespace nurikabe
{
Field::Field() : cells_(), inconsistent_(false), fully_solved_(false)
{
}
Field::Field(const Problem &problem) : cells_(problem.height(), problem.width(), Cell(kNoClue, kCellUndecided)), inconsistent_(false), fully_solved_(false)
{
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			cells_.at(CellPosition(y, x)).group_next_cell = cells_.GetIndex(CellPosition(y, x));

			Clue c = problem.GetClue(CellPosition(y, x));
			if (c != kNoClue) {
				cells_.at(CellPosition(y, x)).clue = c;
			}
		}
	}
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Clue c = problem.GetClue(CellPosition(y, x));
			if (c != kNoClue) {
				DecideCell(CellPosition(y, x), kCellWhite);
			}
		}
	}
}
Field::Field(const Field &other) : cells_(other.cells_), inconsistent_(other.inconsistent_), fully_solved_(other.fully_solved_)
{
}
Field::Field(Field &&other) : cells_(std::move(other.cells_)), inconsistent_(other.inconsistent_), fully_solved_(other.fully_solved_)
{
}
Field::~Field()
{
}
Field &Field::operator=(const Field &other)
{
	cells_ = other.cells_;
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;
	return *this;
}
Field &Field::operator=(Field &&other)
{
	cells_ = std::move(other.cells_);
	inconsistent_ = other.inconsistent_;
	fully_solved_ = other.fully_solved_;
	return *this;
}
void Field::DecideCell(CellPosition pos, CellState status)
{
	Cell &cell = cells_.at(pos);
	if (cell.status != kCellUndecided) {
		if (cell.status != status) {
			SetInconsistent();
		}
		return;
	}

	cell.status = status;
	for (Direction d : k4Neighborhood) {
		CellPosition pos2 = pos + d;
		if (cells_.IsPositionOnGrid(pos2) && cells_.at(pos2).status == status) {
			Join(cells_.GetIndex(pos), cells_.GetIndex(pos2));
		}
	}

	if (status == kCellBlack) {
		// avoid 2*2 cluster of black cells
		CheckCluster(pos - Direction(Y(0), X(0)));
		CheckCluster(pos - Direction(Y(0), X(1)));
		CheckCluster(pos - Direction(Y(1), X(0)));
		CheckCluster(pos - Direction(Y(1), X(1)));
	} else {
		int root_idx = GetRoot(GetIndex(pos));
		Cell &root_cell = cells_.at(root_idx);
		if (root_cell.clue != kNoClue) {
			AvoidGroupWithSeveralClue(pos);

			if (root_cell.clue.clue_high == GetGroupSize(root_idx)) {
				CloseGroup(root_idx);
			} else if (root_cell.clue.clue_high < GetGroupSize(root_idx)) {
				SetInconsistent();
				return;
			}
		}
	}
}
int Field::GetRoot(int cell_idx)
{
	if (cells_.at(cell_idx).group_parent_cell < 0) return cell_idx;
	return cells_.at(cell_idx).group_parent_cell = GetRoot(cells_.at(cell_idx).group_parent_cell);
}
void Field::Join(int cell_idx1, int cell_idx2)
{
	cell_idx1 = GetRoot(cell_idx1);
	cell_idx2 = GetRoot(cell_idx2);

	if (cell_idx1 == cell_idx2) return;

	auto &cell1 = cells_.at(cell_idx1);
	auto &cell2 = cells_.at(cell_idx2);

	if (cell1.clue == kNoClue && (cell2.clue != kNoClue || cell1.group_parent_cell > cell2.group_parent_cell)) {
		cell2.group_parent_cell += cell1.group_parent_cell;
		cell1.group_parent_cell = cell_idx2;
	} else {
		cell1.group_parent_cell += cell2.group_parent_cell;
		cell2.group_parent_cell = cell_idx1;
	}

	std::swap(cell1.group_next_cell, cell2.group_next_cell);
}
void Field::CheckCluster(CellPosition top)
{
	if (!(cells_.IsPositionOnGrid(top) && cells_.IsPositionOnGrid(top + Direction(Y(1), X(1))))) return;

	int cnt = 0;
	if (GetCell(top + Direction(Y(0), X(0))) == kCellBlack) ++cnt;
	if (GetCell(top + Direction(Y(0), X(1))) == kCellBlack) ++cnt;
	if (GetCell(top + Direction(Y(1), X(0))) == kCellBlack) ++cnt;
	if (GetCell(top + Direction(Y(1), X(1))) == kCellBlack) ++cnt;

	if (cnt == 4) {
		SetInconsistent();
		return;
	} else if (cnt == 3) {
		if (GetCell(top + Direction(Y(0), X(0))) == kCellUndecided) DecideCell(top + Direction(Y(0), X(0)), kCellBlack);
		if (GetCell(top + Direction(Y(0), X(1))) == kCellUndecided) DecideCell(top + Direction(Y(0), X(1)), kCellBlack);
		if (GetCell(top + Direction(Y(1), X(0))) == kCellUndecided) DecideCell(top + Direction(Y(1), X(0)), kCellBlack);
		if (GetCell(top + Direction(Y(1), X(1))) == kCellUndecided) DecideCell(top + Direction(Y(1), X(1)), kCellBlack);
	}
}
void Field::AvoidGroupWithSeveralClue(CellPosition pos)
{
	int root_pos = GetRoot(GetIndex(pos));
	if (!HasClueInGroup(root_pos)) return;

	for (int d = 0; d < 4; ++d) {
		Direction dir1 = k4Neighborhood[d], dir2 = k4Neighborhood[(d + 1) % 4];
		{
			CellPosition pos2 = pos + dir1 * 2;
			if (cells_.IsPositionOnGrid(pos2) && GetCell(pos2) == kCellWhite) {
				int root_pos2 = GetRoot(GetIndex(pos2));
				if (HasClueInGroup(root_pos2) && root_pos != root_pos2) {
					DecideCell(pos + dir1, kCellBlack);
				}
			}
		}
		{
			CellPosition pos2 = pos + dir1 + dir2;
			if (cells_.IsPositionOnGrid(pos2) && GetCell(pos2) == kCellWhite) {
				int root_pos2 = GetRoot(GetIndex(pos2));
				if (HasClueInGroup(root_pos2) && root_pos != root_pos2) {
					DecideCell(pos + dir1, kCellBlack);
					DecideCell(pos + dir2, kCellBlack);
				}
			}
		}
	}
}
void Field::CloseGroup(int cell_idx)
{
	int p = cell_idx;
	do {
		CellPosition pos = cells_.AsPosition(p);
		for (Direction d : k4Neighborhood) {
			CellPosition pos2 = pos + d;
			if (cells_.IsPositionOnGrid(pos2) && GetCell(pos2) == kCellUndecided) {
				DecideCell(pos2, kCellBlack);
			}
		}
		p = cells_.at(p).group_next_cell;
	} while (p != cell_idx);
}
}
}