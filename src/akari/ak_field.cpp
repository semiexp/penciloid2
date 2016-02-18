#include "ak_field.h"

#include <vector>

#include "ak_problem.h"

namespace penciloid
{
namespace akari
{
Field::Field() : cells_(), groups_(nullptr), inconsistent_(false)
{
}
Field::Field(const Problem &problem) : cells_(problem.height(), problem.width()), groups_(nullptr), inconsistent_(false)
{
	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			if (problem.GetClue(CellPosition(y, x)) != kEmpty) {
				cells_.at(CellPosition(y, x)).status = CELL_BLOCK;
				cells_.at(CellPosition(y, x)).clue_number = problem.GetClue(CellPosition(y, x));
			} else {
				cells_.at(CellPosition(y, x)).status = CELL_UNDECIDED;
			}
		}
	}
	int n_groups = 0;
	int n_horizontal_groups = 0;

	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			if (problem.GetClue(CellPosition(y, x)) == kEmpty) {
				cells_.at(CellPosition(y, x)).group_horizontal_id = n_groups;
			}
			if ((problem.GetClue(CellPosition(y, x)) != kEmpty || x == problem.width() - 1) && x != 0 && problem.GetClue(CellPosition(y, x - 1)) == kEmpty) {
				++n_groups;
			}
		}
	}
	n_horizontal_groups = n_groups;

	for (X x(0); x < problem.width(); ++x) {
		for (Y y(0); y < problem.height(); ++y) {
			if (problem.GetClue(CellPosition(y, x)) == kEmpty) {
				cells_.at(CellPosition(y, x)).group_vertical_id = n_groups;
			}
			if ((problem.GetClue(CellPosition(y, x)) != kEmpty || y == problem.height() - 1) && y != 0 && problem.GetClue(CellPosition(y - 1, x)) == kEmpty) {
				++n_groups;
			}
		}
	}

	groups_ = new CellGroup[n_groups];
	
	for (int i = 0; i < n_groups; ++i) {
		groups_[i].cell_count = 0;
		groups_[i].xor_remaining_cell_id = 0;
		groups_[i].group_top = CellPosition(Y(-1), X(-1));
		groups_[i].dir = (i < n_horizontal_groups ? GROUP_HORIZONTAL : GROUP_VERTICAL);
	}

	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			if (problem.GetClue(CellPosition(y, x)) != kEmpty) continue;

			int horizontal = cells_.at(CellPosition(y, x)).group_horizontal_id;
			groups_[horizontal].xor_remaining_cell_id ^= cells_.GetIndex(CellPosition(y, x));
			if (groups_[horizontal].cell_count++ == 0) {
				groups_[horizontal].group_top = CellPosition(y, x);
			}

			int vertical = cells_.at(CellPosition(y, x)).group_vertical_id;
			groups_[vertical].xor_remaining_cell_id ^= cells_.GetIndex(CellPosition(y, x));
			if (groups_[vertical].cell_count++ == 0) {
				groups_[vertical].group_top = CellPosition(y, x);
			}
		}
	}

	for (Y y(0); y < cells_.height(); ++y) {
		for (X x(0); x < cells_.width(); ++x) {
			CheckCell(CellPosition(y, x));
		}
	}
}
Field::~Field()
{
	if (groups_) delete[] groups_;
}
Field::CellState Field::GetCell(CellPosition pos) const
{
	return cells_.at(pos).status;
}
Field::CellState Field::GetCellSafe(CellPosition pos) const
{
	if (0 <= pos.y && pos.y < cells_.height() && 0 <= pos.x && pos.x < cells_.width()) return cells_.at(pos).status;
	return CELL_BLOCK;
}
void Field::DecideCell(CellPosition pos, CellState status)
{
	// is the transition possible?
	CellState current_status = GetCell(pos);
	if (current_status == status) return;
	if (current_status == CELL_BLOCK) return;
	if (current_status == CELL_LIGHT) {
		SetInconsistent();
		return;
	}
	if (current_status == CELL_LIT_BY_OTHER) {
		if (status == CELL_NO_LIGHT) return;
		SetInconsistent();
		return;
	}
	if (current_status == CELL_NO_LIGHT_NOT_LIT) {
		if (status == CELL_NO_LIGHT) return;
	}

	if (status == CELL_LIGHT) {
		cells_.at(pos).status = CELL_LIGHT;

		static const Direction kDirs[] = {
			Direction(Y(1), X(0)),
			Direction(Y(0), X(1)),
			Direction(Y(-1), X(0)),
			Direction(Y(0), X(-1))
		};
		for (int d = 0; d < 4; ++d) {
			for (CellPosition p = pos + kDirs[d]; GetCellSafe(p) != CELL_BLOCK; p = p + kDirs[d]) {
				DecideCell(p, CELL_LIT_BY_OTHER);
			}
		}
		CheckNeighbor(pos);
	} else if (status == CELL_LIT_BY_OTHER) {
		if (current_status == CELL_NO_LIGHT_NOT_LIT) {
			cells_.at(pos).status = CELL_LIT_BY_OTHER;
			return;
		}
		cells_.at(pos).status = CELL_LIT_BY_OTHER;
		ExcludeCellFromGroups(pos);
		CheckNeighbor(pos);
	} else if (status == CELL_NO_LIGHT) {
		cells_.at(pos).status = CELL_NO_LIGHT_NOT_LIT;
		ExcludeCellFromGroups(pos);
		CheckNeighbor(pos);
	}
}
void Field::CheckCell(CellPosition pos)
{
	if (!(0 <= pos.y && pos.y < cells_.height() && 0 <= pos.x && pos.x < cells_.width())) return;
	if (cells_.at(pos).status == CELL_BLOCK) {
		// check about the clue of this cell
		int clue = cells_.at(pos).clue_number;
		if (clue < 0) return;

		static const Direction kDirs[] = {
			Direction(Y(1), X(0)),
			Direction(Y(0), X(1)),
			Direction(Y(-1), X(0)),
			Direction(Y(0), X(-1))
		};

		std::vector<int> undecided_dir;
		for (int d = 0; d < 4; ++d) {
			CellState status = GetCellSafe(pos + kDirs[d]);
			if (status == CELL_LIGHT) {
				--clue;
			}
			if (status == CELL_UNDECIDED) {
				undecided_dir.push_back(d);
			}
		}

		if (clue < 0) {
			SetInconsistent();
			return;
		}
		if (clue == 0) {
			for (int d : undecided_dir) {
				DecideCell(pos + kDirs[d], CELL_NO_LIGHT);
			}
		}
		if (clue == undecided_dir.size()) {
			for (int d : undecided_dir) {
				DecideCell(pos + kDirs[d], CELL_LIGHT);
			}
		}
	} else {
		CellState current_status = GetCell(pos);
		if (current_status == CELL_LIGHT || current_status == CELL_LIT_BY_OTHER) return;
		
		// needs to be lit
		int horizontal = cells_.at(pos).group_horizontal_id;
		int vertical = cells_.at(pos).group_vertical_id;

		if (groups_[horizontal].cell_count == 0 && groups_[vertical].cell_count == 1) {
			DecideCell(cells_.AsPosition(groups_[vertical].xor_remaining_cell_id), CELL_LIGHT);
		} else if (groups_[horizontal].cell_count == 1 && groups_[vertical].cell_count == 0) {
			DecideCell(cells_.AsPosition(groups_[horizontal].xor_remaining_cell_id), CELL_LIGHT);
		} else if (groups_[horizontal].cell_count == 1 && groups_[vertical].cell_count == 1) {
			if (groups_[horizontal].xor_remaining_cell_id == groups_[vertical].xor_remaining_cell_id) {
				DecideCell(cells_.AsPosition(groups_[horizontal].xor_remaining_cell_id), CELL_LIGHT);
			}
		}
	}
}
void Field::CheckNeighbor(CellPosition pos)
{
	CheckCell(pos);
	CheckCell(pos + Direction(Y(1), X(0)));
	CheckCell(pos + Direction(Y(-1), X(0)));
	CheckCell(pos + Direction(Y(0), X(1)));
	CheckCell(pos + Direction(Y(0), X(-1)));
}
void Field::CheckGroup(int group_id)
{
	if (groups_[group_id].cell_count >= 2) return;

	if (groups_[group_id].dir == GROUP_HORIZONTAL) {
		for (CellPosition pos = groups_[group_id].group_top; GetCellSafe(pos) != CELL_BLOCK; pos = pos + Direction(Y(0), X(1))) {
			CheckCell(pos);
		}
	} else {
		for (CellPosition pos = groups_[group_id].group_top; GetCellSafe(pos) != CELL_BLOCK; pos = pos + Direction(Y(1), X(0))) {
			CheckCell(pos);
		}
	}
}
void Field::ExcludeCellFromGroups(CellPosition pos)
{
	int id = cells_.GetIndex(pos);

	int horizontal = cells_.at(pos).group_horizontal_id;
	--groups_[horizontal].cell_count;
	groups_[horizontal].xor_remaining_cell_id ^= id;

	int vertical = cells_.at(pos).group_vertical_id;
	--groups_[vertical].cell_count;
	groups_[vertical].xor_remaining_cell_id ^= id;

	CheckGroup(horizontal);
	CheckGroup(vertical);
}
}
}