#include "ak_field.h"

#include <vector>

#include "ak_problem.h"

namespace penciloid
{
namespace akari
{
Field::Field() : cells_(), groups_(nullptr), decided_cells_(0), inconsistent_(false), fully_solved_(false)
{
}
Field::Field(const Problem &problem) : cells_(problem.height(), problem.width()), groups_(nullptr), decided_cells_(0), inconsistent_(false), fully_solved_(false)
{
	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			if (problem.GetClue(CellPosition(y, x)) != kEmpty) {
				cells_(y, x).status = CELL_BLOCK;
				cells_(y, x).clue_number = problem.GetClue(CellPosition(y, x));
				++decided_cells_;
			} else {
				cells_(y, x).status = CELL_UNDECIDED;
			}
		}
	}
	int n_groups = 0;
	int n_horizontal_groups = 0;

	for (Y y(0); y < problem.height(); ++y) {
		for (X x(0); x < problem.width(); ++x) {
			if (problem.GetClue(CellPosition(y, x)) == kEmpty) {
				if (x != 0 && problem.GetClue(CellPosition(y, x - 1)) == kEmpty) {
					cells_(y, x).group_horizontal_id = cells_(y, x - 1).group_horizontal_id;
				} else {
					cells_(y, x).group_horizontal_id = n_groups++;
				}
			}
		}
	}
	n_horizontal_groups = n_groups;

	for (X x(0); x < problem.width(); ++x) {
		for (Y y(0); y < problem.height(); ++y) {
			if (problem.GetClue(CellPosition(y, x)) == kEmpty) {
				if (y != 0 && problem.GetClue(CellPosition(y - 1, x)) == kEmpty) {
					cells_(y, x).group_vertical_id = cells_(y - 1, x).group_vertical_id;
				} else {
					cells_(y, x).group_vertical_id = n_groups++;
				}
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

			int horizontal = cells_(y, x).group_horizontal_id;
			groups_[horizontal].xor_remaining_cell_id ^= cells_.GetIndex(CellPosition(y, x));
			if (groups_[horizontal].cell_count++ == 0) {
				groups_[horizontal].group_top = CellPosition(y, x);
			}

			int vertical = cells_(y, x).group_vertical_id;
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
	return cells_(pos).status;
}
Field::CellState Field::GetCellSafe(CellPosition pos) const
{
	if (0 <= pos.y && pos.y < cells_.height() && 0 <= pos.x && pos.x < cells_.width()) return cells_(pos).status;
	return CELL_BLOCK;
}
Clue Field::GetClueValue(CellPosition pos) const
{
	if (GetCell(pos) == CELL_BLOCK) return cells_(pos).clue_number;
	return kEmpty;
}
void Field::DecideCell(CellPosition pos, CellState status)
{
	// is the transition possible?
	CellState current_status = GetCellSafe(pos);
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
		++decided_cells_;
		if (decided_cells_ == static_cast<int>(height()) * static_cast<int>(width())) {
			fully_solved_ = true;
		}
		cells_(pos).status = CELL_LIGHT;

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
		++decided_cells_;
		if (decided_cells_ == static_cast<int>(height()) * static_cast<int>(width())) {
			fully_solved_ = true;
		}
		if (current_status == CELL_NO_LIGHT_NOT_LIT) {
			cells_(pos).status = CELL_LIT_BY_OTHER;
			return;
		}
		cells_(pos).status = CELL_LIT_BY_OTHER;
		ExcludeCellFromGroups(pos);
		CheckNeighbor(pos);
	} else if (status == CELL_NO_LIGHT) {
		cells_(pos).status = CELL_NO_LIGHT_NOT_LIT;
		ExcludeCellFromGroups(pos);
		CheckNeighbor(pos);
	}
}
void Field::CheckCell(CellPosition pos)
{
	if (!(0 <= pos.y && pos.y < cells_.height() && 0 <= pos.x && pos.x < cells_.width())) return;
	if (cells_(pos).status == CELL_BLOCK) {
		// check about the clue of this cell
		int clue = cells_(pos).clue_number;
		if (clue < 0) return;

		CheckDiagonalChain(pos);

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
		if (clue != 0 && clue == undecided_dir.size() - 1) {
			int u = 0;
			for (int d : undecided_dir) u |= 1 << d;

			for (int d = 0; d < 4; ++d) {
				if ((u & (1 << d)) && (u & (1 << ((d + 1) % 4)))) {
					DecideCell(pos + kDirs[d] + kDirs[(d + 1) % 4], CELL_NO_LIGHT);
				}
			}
		}
	} else {
		CellState current_status = GetCell(pos);
		if (current_status == CELL_LIGHT || current_status == CELL_LIT_BY_OTHER) return;
		
		// needs to be lit
		int horizontal = cells_(pos).group_horizontal_id;
		int vertical = cells_(pos).group_vertical_id;

		if (groups_[horizontal].cell_count == 0 && groups_[vertical].cell_count == 1) {
			DecideCell(cells_.AsPosition(groups_[vertical].xor_remaining_cell_id), CELL_LIGHT);
		} else if (groups_[horizontal].cell_count == 1 && groups_[vertical].cell_count == 0) {
			DecideCell(cells_.AsPosition(groups_[horizontal].xor_remaining_cell_id), CELL_LIGHT);
		} else if (groups_[horizontal].cell_count == 1 && groups_[vertical].cell_count == 1) {
			if (groups_[horizontal].xor_remaining_cell_id == groups_[vertical].xor_remaining_cell_id) {
				DecideCell(cells_.AsPosition(groups_[horizontal].xor_remaining_cell_id), CELL_LIGHT);
			} else {
				CellPosition rem1 = cells_.AsPosition(groups_[horizontal].xor_remaining_cell_id);
				CellPosition rem2 = cells_.AsPosition(groups_[vertical].xor_remaining_cell_id);
				CellPosition common(rem1.y + rem2.y - pos.y, rem1.x + rem2.x - pos.x);
				if (GetCell(common) != CELL_BLOCK) {
					if (cells_(common).group_horizontal_id == cells_(rem2).group_horizontal_id && cells_(common).group_vertical_id == cells_(rem1).group_vertical_id) {
						DecideCell(common, CELL_NO_LIGHT);
					}
				}
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
void Field::CheckDiagonalChain(CellPosition pos)
{
	static const Direction kDirs[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	Clue clue = GetClueValue(pos);
	if (clue == kEmpty || clue == kBlock) return;

	for (int d = 0; d < 4; ++d) {
		Direction dir1 = kDirs[d], dir2 = kDirs[(d + 1) % 4];
		int n_lights = 0;

		CellState state1 = GetCellSafe(pos - dir1);
		if (state1 == CELL_UNDECIDED) continue;
		if (state1 == CELL_LIGHT) ++n_lights;

		CellState state2 = GetCellSafe(pos - dir2);
		if (state2 == CELL_UNDECIDED) continue;
		if (state2 == CELL_LIGHT) ++n_lights;

		CellPosition pos2 = pos;
		while (0 <= pos2.y && pos2.y < height() && 0 <= pos2.x && pos2.x < width()) {
			if (GetCell(pos2) != CELL_BLOCK) break;
			Clue c = GetClueValue(pos2);
			if (c == kBlock) break;

			n_lights = c - n_lights;
			if (n_lights < 0 || n_lights > 2) {
				SetInconsistent();
				return;
			}
			if (n_lights == 0) {
				DecideCell(pos2 + dir1, CELL_NO_LIGHT);
				DecideCell(pos2 + dir2, CELL_NO_LIGHT);
				break;
			}
			if (n_lights == 2) {
				DecideCell(pos2 + dir1, CELL_LIGHT);
				DecideCell(pos2 + dir2, CELL_LIGHT);
				break;
			}

			pos2 = pos2 + dir1 + dir2;
		}
	}
}
void Field::ExcludeCellFromGroups(CellPosition pos)
{
	int id = cells_.GetIndex(pos);

	int horizontal = cells_(pos).group_horizontal_id;
	--groups_[horizontal].cell_count;
	groups_[horizontal].xor_remaining_cell_id ^= id;

	int vertical = cells_(pos).group_vertical_id;
	--groups_[vertical].cell_count;
	groups_[vertical].xor_remaining_cell_id ^= id;

	CheckGroup(horizontal);
	CheckGroup(vertical);
}
std::ostream& operator<<(std::ostream &stream, Field &field)
{
	for (Y y(0); y < field.height(); ++y) {
		for (X x(0); x < field.width(); ++x) {
			Clue c;
			switch (field.GetCell(CellPosition(y, x)))
			{
			case Field::CELL_BLOCK:
				c = field.GetClueValue(CellPosition(y, x));
				if (c == kBlock) stream << "#";
				else stream << static_cast<int>(c);
				break;
			case Field::CELL_LIGHT:
				stream << "*";
				break;
			case Field::CELL_NO_LIGHT_NOT_LIT:
				stream << " ";
				break;
			case Field::CELL_LIT_BY_OTHER:
				stream << "+";
				break;
			case Field::CELL_UNDECIDED:
				stream << ".";
				break;
			}
		}
		stream << std::endl;
	}
	return stream;
}
}
}