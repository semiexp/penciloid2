#include "yj_field.h"

#include <vector>

namespace penciloid
{
namespace yajilin
{
Field::Field() : GridLoop<Field>(), cells_()
{
}
Field::Field(Y height, X width) : GridLoop<Field>(height - 1, width - 1), cells_(height, width, Cell())
{
}
Field::Field(Problem &problem) : GridLoop<Field>(problem.height() - 1, problem.width() - 1), cells_(problem.height(), problem.width(), Cell())
{
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Problem::Cell clue = problem.GetClue(CellPosition(y, x));
			if (clue.direction != Problem::kNoClue) {
				ClueDirection d = kClueNorth;
				switch (clue.direction) {
				case Problem::kClueNorth:
					d = kClueNorth; break;
				case Problem::kClueWest:
					d = kClueWest; break;
				case Problem::kClueEast:
					d = kClueEast; break;
				case Problem::kClueSouth:
					d = kClueSouth; break;
				}
				cells_.at(CellPosition(y, x)) = Cell(kCellClue, d, clue.clue_number);
			}
		}
	}
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			if (GetCellState(CellPosition(y, x)) == kCellClue) Check(LoopPosition(y * 2, x * 2));
		}
	}
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			int adjacent_empty_cells = 0;
			for (Direction d : k4Neighborhood) {
				CellPosition cell2 = CellPosition(y, x) + d;
				if (cell2.y >= 0 && cell2.x >= 0 && cell2.y < height() && cell2.x < width() && GetCellState(cell2) != kCellClue) ++adjacent_empty_cells;
			}
			if (adjacent_empty_cells == 2) {
				for (Direction d : k4Neighborhood) {
					CellPosition cell2 = CellPosition(y, x) + d;
					if (cell2.y >= 0 && cell2.x >= 0 && cell2.y < height() && cell2.x < width() && GetCellState(cell2) != kCellClue) {
						DecideCell(cell2, kCellLine);
					}
				}
			}
		}
	}
}
Field::Field(const Field &other) : GridLoop<Field>(other), cells_(other.cells_)
{
}
Field::Field(Field &&other) : GridLoop<Field>(other), cells_(std::move(other.cells_))
{
}
Field::~Field()
{
}
void Field::DecideCell(CellPosition cell, CellState status)
{
	CellState current_status = cells_.at(cell).status;
	if (current_status != kCellUndecided) {
		if (current_status != status) SetInconsistent();
		return;
	}

	cells_.at(cell).status = status;
	
	for (Direction d : k4Neighborhood) {
		for (CellPosition c = cell + d;; c = c + d) {
			if (c.y < 0 || c.x < 0 || c.y >= height() || c.x >= width()) break;
			Check(LoopPosition(c.y * 2, c.x * 2));
		}
	}

	Check(LoopPosition(cell.y * 2, cell.x * 2));
	if (status == kCellBlock) {
		for (Direction d : k4Neighborhood) {
			CellPosition cell2 = cell + d;
			if (cell2.y >= 0 && cell2.x >= 0 && cell2.y < height() && cell2.x < width() && GetCellState(cell2) != kCellClue) {
				DecideCell(cell2, kCellLine);
			}
		}
	}
}
void Field::Inspect(LoopPosition pos)
{
	// This method works only if cells_ is initialized
	if (cells_.height() == 0) return;

	// Check vertices (of GridLoop, which correspond to cells of Grid) only
	if (!(pos.y % 2 == 0 && pos.x % 2 == 0)) return;
	CellPosition cell(pos.y / 2, pos.x / 2);
	CellState cell_status = cells_.at(cell).status;

	// GridLoop -> Grid
	int n_line = 0, n_undecided = 0;
	for (Direction d : k4Neighborhood) {
		EdgeState s = GetEdgeSafe(pos + d);
		if (s == kEdgeLine) ++n_line;
		else if (s == kEdgeUndecided) ++n_undecided;
	}
	if (n_line > 0) DecideCell(cell, kCellLine);
	if (n_line == 0 && n_undecided == 0 && cell_status != kCellClue) DecideCell(cell, kCellBlock);

	// Grid -> GridLoop
	if ((cell_status == kCellBlock || cell_status == kCellClue) && n_undecided > 0) {
		for (Direction d : k4Neighborhood) {
			DecideEdge(pos + d, kEdgeBlank);
		}
	}
	if (cell_status == kCellLine) {
		if (n_line == 0 && n_undecided == 2) {
			for (Direction d : k4Neighborhood) {
				if (GetEdgeSafe(pos + d) == kEdgeUndecided) DecideEdge(pos + d, kEdgeLine);
			}
		}
	} else if (cell_status == kCellClue) {
		ClueDirection clue_dir = cells_.at(cell).direction;
		Direction dir = GetDirectionValue(clue_dir);
		int clue_num = cells_.at(cell).clue_number;

		int chain_size = 0, n_block = 0, max_extra_block = 0;
		for (CellPosition c = cell + dir;; c = c + dir) {
			if (c.y < 0 || c.x < 0 || c.y >= height() || c.x >= width()) break;
			if (GetCellState(c) == kCellClue) {
				Cell cell_value = GetCell(c);
				if (cell_value.direction == clue_dir) {
					clue_num -= cell_value.clue_number;
					break;
				}
			}

			if (GetCellState(c) == kCellUndecided) {
				++chain_size;
			} else {
				max_extra_block += (chain_size + 1) / 2;
				chain_size = 0;
			}
			if (GetCellState(c) == kCellBlock) {
				--clue_num;
			}
		}
		max_extra_block += (chain_size + 1) / 2;
		chain_size = 0;

		if (clue_num < n_block || n_block + max_extra_block < clue_num) {
			SetInconsistent(); return;
		}
		if (clue_num == n_block) {
			for (CellPosition c = cell + dir;; c = c + dir) {
				if (c.y < 0 || c.x < 0 || c.y >= height() || c.x >= width()) break;
				if (GetCellState(c) == kCellClue && GetCell(c).direction == clue_dir) break;
				if (GetCellState(c) != kCellClue && GetCellState(c) != kCellBlock) DecideCell(c, kCellLine);
			}
		} else {
			if (n_block + max_extra_block == clue_num) {
				std::vector<CellPosition> chain_cells;
				for (CellPosition c = cell + dir;; c = c + dir) {
					if (c.y < 0 || c.x < 0 || c.y >= height() || c.x >= width()) break;
					if (GetCellState(c) == kCellClue && GetCell(c).direction == clue_dir) break;
					if (GetCellState(c) == kCellUndecided) {
						chain_cells.push_back(c);
					} else {
						if (chain_cells.size() % 2 == 1) {
							for (int i = 0; i < chain_cells.size(); i += 2) {
								DecideCell(chain_cells[i], kCellBlock);
							}
						}
						chain_cells.clear();
					}
				}
				if (chain_cells.size() % 2 == 1) {
					for (int i = 0; i < chain_cells.size(); i += 2) {
						DecideCell(chain_cells[i], kCellBlock);
					}
				}
			}
		}
	}
}
Direction Field::GetDirectionValue(ClueDirection dir)
{
	switch (dir)
	{
	case kClueNorth: return Direction(Y(-1), X(0));
	case kClueWest: return Direction(Y(0), X(-1));
	case kClueEast: return Direction(Y(0), X(1));
	case kClueSouth: return Direction(Y(1), X(0));
	}
	return Direction(Y(0), X(0));
}
std::ostream& operator<<(std::ostream &stream, Field &field)
{
	for (Y y(0); y <= 2 * (field.height() - 1); ++y) {
		for (X x(0); x <= 2 * (field.width() - 1); ++x) {
			if (y % 2 == 0 && x % 2 == 0) {
				Field::CellState cell_status = field.GetCellState(CellPosition(y / 2, x / 2));
				if (cell_status == Field::kCellBlock) {
					stream << "#";
				} else if (cell_status == Field::kCellClue) {
					Field::Cell cell = field.GetCell(CellPosition(y / 2, x / 2));
					switch (cell.direction) {
					case Field::kClueNorth: stream << "^"; break;
					case Field::kClueWest: stream << "<"; break;
					case Field::kClueEast: stream << ">"; break;
					case Field::kClueSouth: stream << "v"; break;
					}
					stream << cell.clue_number;
				} else if (cell_status == Field::kCellLine) {
					stream << "+";
				} else {
					stream << ".";
				}
			} else if (y % 2 == 0 && x % 2 == 1) {
				Field::EdgeState status = field.GetEdge(LoopPosition(y, x));
				bool connect_block_or_clue = false;
				if (field.GetCellState(CellPosition(y / 2, x / 2)) == Field::kCellBlock || field.GetCellState(CellPosition(y / 2, x / 2)) == Field::kCellClue) connect_block_or_clue = true;
				if (field.GetCellState(CellPosition(y / 2, x / 2 + 1)) == Field::kCellBlock || field.GetCellState(CellPosition(y / 2, x / 2 + 1)) == Field::kCellClue) connect_block_or_clue = true;
				if (status == Field::kEdgeUndecided || connect_block_or_clue) {
					if (field.GetCellState(CellPosition(y / 2, x / 2)) == Field::kCellClue) stream << "  ";
					else stream << "   ";
				} else if (status == Field::kEdgeLine) stream << "---";
				else if (status == Field::kEdgeBlank) stream << " X ";
			} else if (y % 2 == 1 && x % 2 == 0) {
				Field::EdgeState status = field.GetEdge(LoopPosition(y, x));
				bool connect_block_or_clue = false;
				if (field.GetCellState(CellPosition(y / 2, x / 2)) == Field::kCellBlock || field.GetCellState(CellPosition(y / 2, x / 2)) == Field::kCellClue) connect_block_or_clue = true;
				if (field.GetCellState(CellPosition(y / 2 + 1, x / 2)) == Field::kCellBlock || field.GetCellState(CellPosition(y / 2 + 1, x / 2)) == Field::kCellClue) connect_block_or_clue = true;
				if (status == Field::kEdgeUndecided || connect_block_or_clue) stream << " ";
				else if (status == Field::kEdgeLine) stream << "|";
				else if (status == Field::kEdgeBlank) stream << "X";
			} else if (y % 2 == 1 && x % 2 == 1) {
				stream << "   ";
			}
		}
		stream << "\n";
	}
	return stream;
}
}
}