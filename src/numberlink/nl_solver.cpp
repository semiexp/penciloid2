#include "nl_solver.h"

#include <algorithm>
#include <string>

namespace penciloid
{
namespace numberlink
{
Solver::Solver() :
	height_(0),
	width_(0),
	problem_(),
	answer_(),
	contiguous_line_up_(),
	contiguous_line_left_(),
	contiguous_empty_up_(),
	contiguous_empty_left_(),
	frontier_size_(0)
{
}
Solver::Solver(const Problem &problem) :
	height_(problem.height()),
	width_(problem.width()),
	problem_(problem),
	answer_(problem.height(), problem.width(), 0),
	contiguous_line_up_(problem.height(), problem.width(), 0),
	contiguous_line_left_(problem.height(), problem.width(), 0),
	contiguous_empty_up_(problem.height(), problem.width(), 0),
	contiguous_empty_left_(problem.height(), problem.width(), 0),
	frontier_size_(problem.width())
{
}
Solver::~Solver()
{
}
void Solver::SolveBySearch()
{
	CellState tmp[21];
	for (X x(0); x < problem_.width(); ++x) {
		int clue = static_cast<int>(problem_.GetClue(CellPosition(Y(0), x)));
		if (clue == 0) tmp[x] = x;
		else tmp[x] = clue + frontier_size_;
	}

	Search(Y(0), X(0), tmp);
}
void Solver::SolveBySearch2()
{
	Field field(problem_);
	Search2(Y(0), X(0), field);
}
void Solver::Search(Y y, X x, Frontier frontier)
{
	if (x == problem_.width()) {
		x = 0;
		++y;
	}
	if (y == problem_.height()) {
		std::vector<std::string> sol(height() * 2, std::string(width() * 2, ' '));
		for (Y y(0); y < height(); ++y) {
			for (X x(0); x < width(); ++x) {
				int c = static_cast<int>(problem_.GetClue(CellPosition(y, x)));
				if (c >= 1) {
					if (c <= 9) sol[y * 2][x * 2] = c + '0';
					else if (c <= 36) sol[y * 2][x * 2] = c - 10 + 'a';
				} else sol[y * 2][x * 2] = '+';
				int a = answer_.at(CellPosition(y, x));
				if (a & 1) sol[y * 2][x * 2 + 1] = '-';
				if (a & 2) sol[y * 2 + 1][x * 2] = '|';
			}
		}
		return;
	}

	if (y > 0 && x > 0) {
		int line_up = contiguous_line_up_.at(CellPosition(y, x));
		int line_left = contiguous_line_left_.at(CellPosition(y, x));
		if (line_up >= 1) {
			if (line_left >= 1) {
				if ((answer_.at(CellPosition(y - 1, x - line_left)) & 2) && contiguous_empty_left_.at(CellPosition(y - 1, x - 1)) == line_left - 1) return;
			} else {
				int line_left2 = contiguous_line_left_.at(CellPosition(y - 1, x));
				if ((answer_.at(CellPosition(y - 1, x - line_left2)) & 2) && contiguous_empty_left_.at(CellPosition(y, x - 1)) == line_left2 - 1) return;
			}
		}
		if (line_left >= 1) {
			if (line_up >= 1) {
				if ((answer_.at(CellPosition(y - line_up, x - 1)) & 1) && contiguous_empty_up_.at(CellPosition(y - 1, x - 1)) == line_up - 1) return;
			} else {
				int line_up2 = contiguous_line_up_.at(CellPosition(y, x - 1));
				if ((answer_.at(CellPosition(y - line_up2, x - 1)) & 1) && contiguous_empty_up_.at(CellPosition(y - 1, x)) == line_up2 - 1) return;
			}
		}
		if ((answer_.at(CellPosition(y, x - 1)) & 1) && (answer_.at(CellPosition(y - 1, x - 1)) & 2)) {
			if (answer_.at(CellPosition(y - 1, x)) == 0 && !(answer_.at(CellPosition(y - 1, x - 1)) & 1) && (y == 1 || !(answer_.at(CellPosition(y - 2, x)) & 2))) {
				return;
			}
		}
		if ((answer_.at(CellPosition(y, x - 1)) & 1) && (answer_.at(CellPosition(y - 1, x)) & 2)) {
			if (answer_.at(CellPosition(y - 1, x - 1)) == 0 && (x == 1 || !(answer_.at(CellPosition(y - 1, x - 2)) & 1)) && (y == 1 || !(answer_.at(CellPosition(y - 2, x - 1)) & 2))) {
				return;
			}
		}
	}
	contiguous_empty_up_.at(CellPosition(y, x)) = contiguous_empty_left_.at(CellPosition(y, x)) = 0;

	CellState tmp[25];
	if (x < width() - 1) {
		CopyFrontier(frontier, tmp);
		if (Join(tmp, x, x + 1)) {
			contiguous_line_left_.at(CellPosition(y, x + 1)) = contiguous_line_left_.at(CellPosition(y, x)) + 1;
			if (tmp[x] == x || tmp[x] == frontier_size_) {
				// cut here
				answer_.at(CellPosition(y, x)) = 1;
				if (y < height() - 1) contiguous_line_up_.at(CellPosition(y + 1, x)) = 0;
				if (y < problem_.height() - 1) {
					int clue = static_cast<int>(problem_.GetClue(CellPosition(y + 1, x)));
					if (clue == 0) tmp[x] = x;
					else tmp[x] = clue + frontier_size_;
					Search(y, x + 1, tmp);
				} else {
					tmp[x] = frontier_size_;
					Search(y, x + 1, tmp);
				}
			} else if (y < height() - 1) {
				int clue = static_cast<int>(problem_.GetClue(CellPosition(y + 1, x)));
				answer_.at(CellPosition(y, x)) = 3;
				contiguous_line_up_.at(CellPosition(y + 1, x)) = contiguous_line_up_.at(CellPosition(y, x)) + 1;
				if (x >= 1 && answer_.at(CellPosition(y, x - 1)) == 3) {
				} else {
					if (clue != 0 && (tmp[x] > frontier_size_ && tmp[x] != clue + frontier_size_)) {
					} else {
						if (clue == 0) {
							Search(y, x + 1, tmp);
						} else {
							if (tmp[x] < frontier_size_) {
								tmp[tmp[x]] = clue + frontier_size_;
								tmp[x] = frontier_size_;
							} else {
								tmp[x] = frontier_size_;
							}
							Search(y, x + 1, tmp);
						}
					}
				}
			}
		}
	}
	{
		CopyFrontier(frontier, tmp);
		if (x < width() - 1) contiguous_line_left_.at(CellPosition(y, x + 1)) = 0;
		if (tmp[x] == x || tmp[x] == frontier_size_) {
			// cut here
			answer_.at(CellPosition(y, x)) = 0;
			if (y < height() - 1) contiguous_line_up_.at(CellPosition(y + 1, x)) = 0;
			if ((y == 0 || !(answer_.at(CellPosition(y - 1, x)) & 2)) && (x == 0 || !(answer_.at(CellPosition(y, x - 1)) & 1))) {
				contiguous_empty_up_.at(CellPosition(y, x)) = (y == 0 ? 0 : contiguous_empty_up_.at(CellPosition(y - 1, x))) + 1;
				contiguous_empty_left_.at(CellPosition(y, x)) = (x == 0 ? 0 : contiguous_empty_left_.at(CellPosition(y, x - 1))) + 1;
			}
			if (y < problem_.height() - 1) {
				int clue = static_cast<int>(problem_.GetClue(CellPosition(y + 1, x)));
				if (clue == 0) tmp[x] = x;
				else tmp[x] = clue + frontier_size_;
				Search(y, x + 1, tmp);
			} else {
				tmp[x] = frontier_size_;
				Search(y, x + 1, tmp);
			}
		} else if (y < height() - 1) {
			int clue = static_cast<int>(problem_.GetClue(CellPosition(y + 1, x)));
			answer_.at(CellPosition(y, x)) = 2;
			contiguous_line_up_.at(CellPosition(y + 1, x)) = contiguous_line_up_.at(CellPosition(y, x)) + 1;
			if (x >= 1 && answer_.at(CellPosition(y, x - 1)) == 3) {
			} else {
				if (clue != 0 && (tmp[x] > frontier_size_ && tmp[x] != clue + frontier_size_)) {
				} else {
					if (clue == 0) {
						Search(y, x + 1, tmp);
					} else {
						if (tmp[x] < frontier_size_) {
							tmp[tmp[x]] = clue + frontier_size_;
							tmp[x] = frontier_size_;
						} else {
							tmp[x] = frontier_size_;
						}
						Search(y, x + 1, tmp);
					}
				}
			}
		}
	}
	answer_.at(CellPosition(y, x)) = 0;
}
void Solver::Search2(Y y, X x, Field &field)
{
	if (x == problem_.width()) {
		x = 0;
		++y;
	}
	if (y == problem_.height()) {
		std::vector<std::string> sol(height() * 2, std::string(width() * 2, ' '));
		for (Y y(0); y < height(); ++y) {
			for (X x(0); x < width(); ++x) {
				int c = static_cast<int>(problem_.GetClue(CellPosition(y, x)));
				if (c >= 1) {
					if (c <= 9) sol[y * 2][x * 2] = c + '0';
					else if (c <= 36) sol[y * 2][x * 2] = c - 10 + 'a';
				} else sol[y * 2][x * 2] = '+';

				if (y != height() - 1 && field.GetVerticalLine(CellPosition(y, x)) == Field::kEdgeLine) sol[y * 2 + 1][x * 2] = '|';
				if (x != width() - 1 && field.GetHorizontalLine(CellPosition(y, x)) == Field::kEdgeLine) sol[y * 2][x * 2 + 1] = '-';
			}
		}
		return;
	}
	int incident_lines = 0;
	if (problem_.GetClue(CellPosition(y, x)) != kNoClue) ++incident_lines;
	if (y > 0 && field.GetVerticalLine(CellPosition(y - 1, x)) == Field::kEdgeLine) ++incident_lines;
	if (x > 0 && field.GetHorizontalLine(CellPosition(y, x - 1)) == Field::kEdgeLine) ++incident_lines;

	// pruning
	if (y > 0 && x > 0) {
		if (field.GetVerticalLine(CellPosition(y - 1, x)) == Field::kEdgeLine) {
			for (X x2 = x - 1; x2 >= 0; --x2) {
				if (field.GetHorizontalLine(CellPosition(y - 1, x2)) != Field::kEdgeLine) break;
				if (!field.IsIsolatedCell(CellPosition(y, x2))) {
					if (field.GetVerticalLine(CellPosition(y - 1, x2)) == Field::kEdgeLine) return;
					else break;
				}
			}
		}
		if (field.GetHorizontalLine(CellPosition(y - 1, x - 1)) == Field::kEdgeLine
			&& field.GetHorizontalLine(CellPosition(y, x - 1)) == Field::kEdgeLine
			&& (field.GetVerticalLine(CellPosition(y - 1, x - 1)) == Field::kEdgeLine || field.GetVerticalLine(CellPosition(y - 1, x)) == Field::kEdgeLine)) {
			return;
		}
		if (field.GetVerticalLine(CellPosition(y - 1, x - 1)) == Field::kEdgeLine
			&& field.GetVerticalLine(CellPosition(y - 1, x)) == Field::kEdgeLine
			&& (field.GetHorizontalLine(CellPosition(y - 1, x - 1)) == Field::kEdgeLine || field.GetHorizontalLine(CellPosition(y, x - 1)) == Field::kEdgeLine)) {
			return;
		}
		if (field.GetVerticalLine(CellPosition(y - 1, x - 1)) == Field::kEdgeLine
			&& field.GetHorizontalLine(CellPosition(y, x - 1)) == Field::kEdgeLine
			&& field.IsIsolatedCell(CellPosition(y - 1, x))) {
			return;
		}
		if (field.GetVerticalLine(CellPosition(y - 1, x)) == Field::kEdgeLine
			&& field.GetHorizontalLine(CellPosition(y, x - 1)) == Field::kEdgeLine
			&& field.IsIsolatedCell(CellPosition(y - 1, x - 1))) {
			return;
		}
	}
	if (field.GetHorizontalLine(CellPosition(y, x)) != Field::kEdgeUndecided && field.GetVerticalLine(CellPosition(y, x)) != Field::kEdgeUndecided) {
		Search2(y, x + 1, field);
		return;
	}
	if (incident_lines % 2 == 1) {
		if (x < width() - 1) {
			field.AddRestorePoint();
			field.SetHorizontalLine(CellPosition(y, x));
			field.SetVerticalBlank(CellPosition(y, x));
			if (!field.IsInconsistent()) {
				Search2(y, x + 1, field);
			}
			field.Restore();
		}
		if (y < height() - 1) {
			if (x > 0 && field.GetHorizontalLine(CellPosition(y, x - 1)) == Field::kEdgeLine && field.GetVerticalLine(CellPosition(y, x - 1)) == Field::kEdgeLine) return;
			field.AddRestorePoint();
			field.SetHorizontalBlank(CellPosition(y, x));
			field.SetVerticalLine(CellPosition(y, x));
			if (!field.IsInconsistent()) {
				Search2(y, x + 1, field);
			}
			field.Restore();
		}
	} else {
		if (x < width() - 1 && y < height() - 1) {
			field.AddRestorePoint();
			field.SetHorizontalLine(CellPosition(y, x));
			field.SetVerticalLine(CellPosition(y, x));
			if (!field.IsInconsistent()) {
				Search2(y, x + 1, field);
			}
			field.Restore();
		}
		{
			field.AddRestorePoint();
			field.SetHorizontalBlank(CellPosition(y, x));
			field.SetVerticalBlank(CellPosition(y, x));
			if (!field.IsInconsistent()) {
				Search2(y, x + 1, field);
			}
			field.Restore();
		}
	}
}
bool Solver::Join(Frontier f, int i, int j)
{
	if (i == j || f[i] == j) return false;
	if (f[i] == frontier_size_ || f[j] == frontier_size_) return false;
	if (f[i] > frontier_size_) {
		if (f[j] > frontier_size_) {
			if (f[i] == f[j]) {
				f[i] = f[j] = frontier_size_;
				return true;
			}
			return false;
		}
		if (f[j] == j) {
			f[j] = f[i];
			f[i] = frontier_size_;
		} else {
			f[f[j]] = f[i];
			f[i] = f[j] = frontier_size_;
		}
		return true;
	}
	if (f[j] > frontier_size_) {
		if (f[i] == i) {
			f[i] = f[j];
			f[j] = frontier_size_;
		} else {
			f[f[i]] = f[j];
			f[i] = f[j] = frontier_size_;
		}
		return true;
	}
	if (f[i] == i) {
		f[i] = f[j];
		if (f[j] == j) {
			f[j] = i;
		} else {
			f[f[j]] = i;
			f[j] = frontier_size_;
		}
		return true;
	}
	if (f[j] == j) {
		f[f[i]] = j;
		f[j] = f[i];
		f[i] = frontier_size_;
		return true;
	}
	f[f[i]] = f[j];
	f[f[j]] = f[i];
	f[i] = f[j] = frontier_size_;
	return true;
}
}
}