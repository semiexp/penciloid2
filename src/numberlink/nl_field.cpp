#include "nl_field.h"

namespace penciloid
{
namespace numberlink
{
Field::Field() : history_(), mate_(), line_horizontal_(), line_vertical_(), endpoint_(), inconsistent_(false)
{
}
Field::Field(const Problem &problem) :
	history_(),
	mate_(problem.height(), problem.width(), 0),
	line_horizontal_(problem.height(), problem.width(), kEdgeUndecided),
	line_vertical_(problem.height(), problem.width(), kEdgeUndecided),
	endpoint_(problem.height(), problem.width(), false),
	inconsistent_(false)
{
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			Clue clue = problem.GetClue(CellPosition(y, x));
			if (clue == kNoClue) {
				mate_.at(CellPosition(y, x)) = mate_.GetIndex(CellPosition(y, x));
			} else {
				endpoint_.at(CellPosition(y, x)) = true;
				mate_.at(CellPosition(y, x)) = -clue;
			}
			if (y == height() - 1) line_vertical_.at(CellPosition(y, x)) = kEdgeBlank;
			if (x == width() - 1) line_horizontal_.at(CellPosition(y, x)) = kEdgeBlank;
		}
	}
}
void Field::SetHorizontalLine(CellPosition cell)
{
	if (GetHorizontalLine(cell) == kEdgeLine) return;
	if (GetHorizontalLine(cell) == kEdgeBlank) {
		SetInconsistent();
		return;
	}
	Join(cell, cell + Direction(Y(0), X(1)));
	if (IsInconsistent()) return;

	UpdateLineHorizontal(GetIndex(cell), kEdgeLine);

	if (cell.y > 0) {
		if (GetHorizontalLine(cell + Direction(Y(-1), X(0))) == kEdgeLine) {
			SetVerticalBlank(cell + Direction(Y(-1), X(0)));
			SetVerticalBlank(cell + Direction(Y(-1), X(1)));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(cell + Direction(Y(-1), X(0))) == kEdgeLine) {
			SetHorizontalBlank(cell + Direction(Y(-1), X(0)));
			SetVerticalBlank(cell + Direction(Y(-1), X(1)));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(cell + Direction(Y(-1), X(1))) == kEdgeLine) {
			SetHorizontalBlank(cell + Direction(Y(-1), X(0)));
			SetVerticalBlank(cell + Direction(Y(-1), X(0)));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(cell + Direction(Y(1), X(0))) == kEdgeLine) {
			SetVerticalBlank(cell + Direction(Y(0), X(0)));
			SetVerticalBlank(cell + Direction(Y(0), X(1)));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(cell + Direction(Y(0), X(0))) == kEdgeLine) {
			SetHorizontalBlank(cell + Direction(Y(1), X(0)));
			SetVerticalBlank(cell + Direction(Y(0), X(1)));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(cell + Direction(Y(0), X(1))) == kEdgeLine) {
			SetHorizontalBlank(cell + Direction(Y(1), X(0)));
			SetVerticalBlank(cell + Direction(Y(0), X(0)));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(cell + Direction(Y(-1), X(0))) == kEdgeLine) {
			if (cell.x == width() - 1) {
				SetInconsistent();
				return;
			}
			if (!endpoint_.at(cell + Direction(Y(-1), X(1)))) {
				if (cell.y > 1 && cell.x < width() - 2) {
					SetVerticalLine(cell + Direction(Y(-2), X(1)));
					SetHorizontalLine(cell + Direction(Y(-1), X(1)));
				} else {
					SetInconsistent();
					return;
				}
			}
		}
		if (GetVerticalLine(cell + Direction(Y(-1), X(1))) == kEdgeLine) {
			if (!endpoint_.at(cell + Direction(Y(-1), X(0)))) {
				if (cell.y > 1 && cell.x > 0) {
					SetVerticalLine(cell + Direction(Y(-2), X(0)));
					SetHorizontalLine(cell + Direction(Y(-1), X(-1)));
				} else {
					SetInconsistent();
					return;
				}
			}
		}
		if (IsInconsistent()) return;
	}
	Inspect(cell);
	if (IsInconsistent()) return;
	Inspect(cell + Direction(Y(0), X(1)));
}
void Field::SetVerticalLine(CellPosition cell)
{
	if (GetVerticalLine(cell) == kEdgeLine) return;
	if (GetVerticalLine(cell) == kEdgeBlank) {
		SetInconsistent();
		return;
	}
	Join(cell, cell + Direction(Y(1), X(0)));
	if (IsInconsistent()) return;

	UpdateLineVertical(GetIndex(cell), kEdgeLine);

	if (cell.x > 0) {
		if (GetVerticalLine(cell + Direction(Y(0), X(-1))) == kEdgeLine) {
			SetHorizontalBlank(cell + Direction(Y(0), X(-1)));
			SetHorizontalBlank(cell + Direction(Y(1), X(-1)));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(cell + Direction(Y(0), X(-1))) == kEdgeLine) {
			SetVerticalBlank(cell + Direction(Y(0), X(-1)));
			SetHorizontalBlank(cell + Direction(Y(1), X(-1)));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(cell + Direction(Y(1), X(-1))) == kEdgeLine) {
			SetVerticalBlank(cell + Direction(Y(0), X(-1)));
			SetHorizontalBlank(cell + Direction(Y(0), X(-1)));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(cell + Direction(Y(0), X(1))) == kEdgeLine) {
			SetHorizontalBlank(cell + Direction(Y(0), X(0)));
			SetHorizontalBlank(cell + Direction(Y(1), X(0)));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(cell + Direction(Y(0), X(0))) == kEdgeLine) {
			SetVerticalBlank(cell + Direction(Y(0), X(1)));
			SetHorizontalBlank(cell + Direction(Y(1), X(0)));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(cell + Direction(Y(1), X(0))) == kEdgeLine) {
			SetVerticalBlank(cell + Direction(Y(0), X(1)));
			SetHorizontalBlank(cell + Direction(Y(0), X(0)));
			if (IsInconsistent()) return;
		}
		if (cell.y < height() - 1 && GetHorizontalLine(cell + Direction(Y(1), X(-1))) == kEdgeLine) {
			if (!endpoint_.at(cell + Direction(Y(0), X(-1)))) {
				if (cell.y > 0 && cell.x > 1) {
					SetVerticalLine(cell + Direction(Y(-1), X(-1)));
					SetHorizontalLine(cell + Direction(Y(0), X(-2)));
				} else {
					SetInconsistent();
					return;
				}
			}
		}
		if (cell.y < height() - 1 && GetHorizontalLine(cell + Direction(Y(1), X(0))) == kEdgeLine) {
			if (!endpoint_.at(cell + Direction(Y(0), X(1)))) {
				if (cell.y > 0 && cell.x < width() - 1) {
					SetVerticalLine(cell + Direction(Y(-1), X(1)));
					SetHorizontalLine(cell + Direction(Y(0), X(1)));
				} else {
					SetInconsistent();
					return;
				}
			}
		}
		if (IsInconsistent()) return;
	}
	Inspect(cell);
	if (IsInconsistent()) return;
	Inspect(cell + Direction(Y(1), X(0)));
}
void Field::SetHorizontalBlank(CellPosition cell)
{
	if (GetHorizontalLine(cell) == kEdgeBlank) return;
	if (GetHorizontalLine(cell) == kEdgeLine) {
		SetInconsistent();
		return;
	}
	int idx = GetIndex(cell);
	UpdateLineHorizontal(idx, kEdgeBlank);
	Inspect(cell);
	if (IsInconsistent()) return;
	Inspect(cell + Direction(Y(0), X(1)));
}
void Field::SetVerticalBlank(CellPosition cell)
{
	if (GetVerticalLine(cell) == kEdgeBlank) return;
	if (GetVerticalLine(cell) == kEdgeLine) {
		SetInconsistent();
		return;
	}
	int idx = GetIndex(cell);
	UpdateLineVertical(idx, kEdgeBlank);
	Inspect(cell);
	if (IsInconsistent()) return;
	Inspect(cell + Direction(Y(1), X(0)));
}
void Field::Restore()
{
	while (!history_.empty()) {
		auto data = history_.back();
		history_.pop_back();

		if (data.first == 4) break;

		switch (data.first & 7) {
		case 0:
			mate_.at(data.first >> 3) = data.second;
			break;
		case 1:
			line_horizontal_.at(data.first >> 3) = static_cast<EdgeState>(data.second);
			break;
		case 2:
			line_vertical_.at(data.first >> 3) = static_cast<EdgeState>(data.second);
			break;
		case 3:
			inconsistent_ = (data.second != 0 ? true : false);
			break;
		}
	}
}
void Field::Inspect(CellPosition cell)
{
	Y y = cell.y; X x = cell.x;
	int n_lines = 0, n_blanks = 0;
	bool is_end = endpoint_.at(cell);
	if (!is_end) {
		int mate = mate_.at(cell);
		if (mate == kFullyConnectedCell || mate == GetIndex(cell)) return;
	}
	if (x == 0) ++n_blanks;
	else {
		EdgeState status = GetHorizontalLine(CellPosition(y, x - 1));
		if (status == kEdgeLine) ++n_lines;
		else if (status == kEdgeBlank) ++n_blanks;
		else {
			int mate1 = mate_.at(cell), mate2 = mate_.at(CellPosition(y, x - 1));
			if (mate1 < 0 && mate2 < 0 && mate1 != mate2) {
				SetHorizontalBlank(CellPosition(y, x - 1));
				return;
			}
		}
	}
	if (y == 0) ++n_blanks;
	else {
		EdgeState status = GetVerticalLine(CellPosition(y - 1, x));
		if (status == kEdgeLine) ++n_lines;
		else if (status == kEdgeBlank) ++n_blanks;
		else {
			int mate1 = mate_.at(cell), mate2 = mate_.at(CellPosition(y - 1, x));
			if (mate1 < 0 && mate2 < 0 && mate1 != mate2) {
				SetVerticalBlank(CellPosition(y - 1, x));
				return;
			}
		}
	}
	{
		EdgeState status = GetHorizontalLine(CellPosition(y, x));
		if (status == kEdgeLine) ++n_lines;
		else if (status == kEdgeBlank) ++n_blanks;
		else {
			int mate1 = mate_.at(cell), mate2 = mate_.at(CellPosition(y, x + 1));
			if (mate1 < 0 && mate2 < 0 && mate1 != mate2) {
				SetHorizontalBlank(CellPosition(y, x));
				return;
			}
		}
	}
	{
		EdgeState status = GetVerticalLine(CellPosition(y, x));
		if (status == kEdgeLine) ++n_lines;
		else if (status == kEdgeBlank) ++n_blanks;
		else {
			int mate1 = mate_.at(cell), mate2 = mate_.at(CellPosition(y + 1, x));
			if (mate1 < 0 && mate2 < 0 && mate1 != mate2) {
				SetVerticalBlank(CellPosition(y, x));
				return;
			}
		}
	}
	if (n_lines == 1 && !is_end && n_blanks == 3) {
		SetInconsistent();
		return;
	}
	if (n_lines == 2 || (n_lines == 1 && is_end)) {
		if (x > 0 && GetHorizontalLine(CellPosition(y, x - 1)) == kEdgeUndecided) {
			SetHorizontalBlank(CellPosition(y, x - 1));
			if (IsInconsistent()) return;
		}
		if (y > 0 && GetVerticalLine(CellPosition(y - 1, x)) == kEdgeUndecided) {
			SetVerticalBlank(CellPosition(y - 1, x));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(CellPosition(y, x)) == kEdgeUndecided) {
			SetHorizontalBlank(CellPosition(y, x));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(CellPosition(y, x)) == kEdgeUndecided) {
			SetVerticalBlank(CellPosition(y, x));
			if (IsInconsistent()) return;
		}
	}
	if ((n_lines == 1 && !is_end && n_blanks == 2) || (n_lines == 0 && is_end && n_blanks == 3)) {
		if (x > 0 && GetHorizontalLine(CellPosition(y, x - 1)) == kEdgeUndecided) {
			SetHorizontalLine(CellPosition(y, x - 1));
			if (IsInconsistent()) return;
		}
		if (y > 0 && GetVerticalLine(CellPosition(y - 1, x)) == kEdgeUndecided) {
			SetVerticalLine(CellPosition(y - 1, x));
			if (IsInconsistent()) return;
		}
		if (GetHorizontalLine(CellPosition(y, x)) == kEdgeUndecided) {
			SetHorizontalLine(CellPosition(y, x));
			if (IsInconsistent()) return;
		}
		if (GetVerticalLine(CellPosition(y, x)) == kEdgeUndecided) {
			SetVerticalLine(CellPosition(y, x));
			if (IsInconsistent()) return;
		}
	}
}
void Field::Join(CellPosition cell1, CellPosition cell2)
{
	int id1 = mate_.GetIndex(cell1), id2 = mate_.GetIndex(cell2);
	int val1 = mate_.at(id1), val2 = mate_.at(id2);

	if (val1 == kFullyConnectedCell || val2 == kFullyConnectedCell || id1 == val2) {
		SetInconsistent();
		return;
	}
	if (val1 < 0 && val2 < 0) {
		if (val1 == val2) {
			UpdateMate(id1, kFullyConnectedCell);
			UpdateMate(id2, kFullyConnectedCell);
		} else {
			SetInconsistent();
		}
		return;
	}
	if (val1 < 0) {
		if (id2 == val2) {
			UpdateMate(val2, val1);
		} else {
			UpdateMate(val2, val1);
			UpdateMate(id2, kFullyConnectedCell);
		}
		UpdateMate(id1, kFullyConnectedCell);
		return;
	}
	if (val2 < 0) {
		if (id1 == val1) {
			UpdateMate(val1, val2);
		} else {
			UpdateMate(val1, val2);
			UpdateMate(id1, kFullyConnectedCell);
		}
		UpdateMate(id2, kFullyConnectedCell);
		return;
	}
	bool eq1 = (val1 == id1), eq2 = (val2 == id2);
	if (eq1) {
		UpdateMate(val1, val2);
	} else {
		UpdateMate(val1, val2);
		UpdateMate(id1, kFullyConnectedCell);
	}
	if (eq2) {
		UpdateMate(val2, val1);
	} else {
		UpdateMate(val2, val1);
		UpdateMate(id2, kFullyConnectedCell);
	}
}
}
}