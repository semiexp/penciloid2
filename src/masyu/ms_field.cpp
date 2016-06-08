#include "ms_field.h"

#include <utility>

namespace penciloid
{
namespace masyu
{
Field::Field() : GridLoop<Field>(), clues_()
{
}
Field::Field(Y height, X width) : GridLoop<Field>(height - 1, width - 1), clues_(height, width, kNoClue)
{
}
Field::Field(Problem &problem) : GridLoop<Field>(problem.height() - 1, problem.width() - 1), clues_(problem.height(), problem.width())
{
	QueuedRun([&]() {
		for (Y y(0); y < problem.height(); ++y) {
			for (X x(0); x < problem.width(); ++x) {
				CellPosition pos(y, x);
				if (problem.GetClue(pos) != kNoClue) {
					AddClue(pos, problem.GetClue(pos));
				}
			}
		}
	});
}
Field::Field(const Field &other) : GridLoop<Field>(other), clues_(other.clues_)
{
}
Field::Field(Field &&other) : GridLoop<Field>(other), clues_(std::move(other.clues_))
{
}
Field::~Field()
{
}
Field &Field::operator=(const Field &other)
{
	GridLoop<Field>::operator=(other);
	clues_ = other.clues_;

	return *this;
}
Field &Field::operator=(Field &&other)
{
	GridLoop<Field>::operator=(other);
	clues_ = std::move(other.clues_);

	return *this;
}
void Field::AddClue(CellPosition cell, Clue clue)
{
	if (GetClue(cell) != kNoClue) {
		if (GetClue(cell) != clue) {
			// Two different kinds of clues are incompatible, so they cause inconsistency.
			SetInconsistent();
		}
		return;
	}
	clues_.at(cell) = clue;
	ApplyTheorem(LoopPosition(cell.y * 2, cell.x * 2));
	Check(LoopPosition(cell.y * 2, cell.x * 2));
}
void Field::Inspect(LoopPosition pos)
{
	if (pos.y % 2 != 0 || pos.x % 2 != 0) return;

	CellPosition clue_pos(pos.y / 2, pos.x / 2);
	
	if (GetClue(clue_pos) == kClueBlack) {
		for (int i = 0; i < 4; ++i) {
			Direction dir = k4Neighborhood[i], dir_cross = k4Neighborhood[(i + 1) % 4];
			if (GetEdgeSafe(pos + dir) == kEdgeBlank || GetEdgeSafe(pos + dir * 3) == kEdgeBlank || GetEdgeSafe(pos + dir * 2 + dir_cross) == kEdgeLine || GetEdgeSafe(pos + dir * 2 - dir_cross) == kEdgeLine) {
				DecideEdge(pos - dir, kEdgeLine);
				DecideEdge(pos - dir * 3, kEdgeLine);
				DecideEdge(pos + dir, kEdgeBlank);
			}
		}
	} else if (GetClue(clue_pos) == kClueWhite) {
		for (int i = 0; i < 2; ++i) {
			Direction dir = k4Neighborhood[i], dir_cross = k4Neighborhood[i + 1];
			if (GetEdgeSafe(pos + dir) == kEdgeLine || GetEdgeSafe(pos - dir) == kEdgeLine) {
				DecideEdge(pos + dir, kEdgeLine);
				DecideEdge(pos - dir, kEdgeLine);

				if (GetEdgeSafe(pos + dir * 3) == kEdgeLine) DecideEdge(pos - dir * 3, kEdgeBlank);
				if (GetEdgeSafe(pos - dir * 3) == kEdgeLine) DecideEdge(pos + dir * 3, kEdgeBlank);
			}
			if (GetEdgeSafe(pos + dir) == kEdgeBlank || GetEdgeSafe(pos - dir) == kEdgeBlank || (GetEdgeSafe(pos + 3 * dir) == kEdgeLine && GetEdgeSafe(pos - 3 * dir) == kEdgeLine)) {
				DecideEdge(pos + dir_cross, kEdgeLine);
				DecideEdge(pos - dir_cross, kEdgeLine);
			}
		}
	}
}
void Field::ApplyTheorem(LoopPosition pos)
{
	CellPosition cell(pos.y / 2, pos.x / 2);
	Clue clue = GetClue(cell);

	if (clue == kClueBlack) {
		for (Direction dir : k4Neighborhood) {
			CellPosition cell2 = cell + dir;
			if (clues_.IsPositionOnGrid(cell2) && GetClue(cell2) == kClueBlack) {
				DecideEdge(pos - dir, kEdgeLine);
				DecideEdge(pos - 3 * dir, kEdgeLine);
				DecideEdge(pos + 3 * dir, kEdgeLine);
				DecideEdge(pos + 5 * dir, kEdgeLine);
			}
		}
	} else if (clue == kClueWhite) {
		for (int i = 0; i < 2; ++i) {
			Direction dir = k4Neighborhood[i], dir_cross = k4Neighborhood[i + 1];

			CellPosition cell2 = cell + dir, cell3 = cell + 2 * dir;
			CellPosition cell_m2 = cell - dir, cell_m3 = cell - 2 * dir;
			bool is_triple = false;

			if (clues_.IsPositionOnGrid(cell2) && clues_.IsPositionOnGrid(cell3) && GetClue(cell2) == kClueWhite && GetClue(cell3) == kClueWhite) is_triple = true;
			if (clues_.IsPositionOnGrid(cell2) && clues_.IsPositionOnGrid(cell_m2) && GetClue(cell2) == kClueWhite && GetClue(cell_m2) == kClueWhite) is_triple = true;
			if (clues_.IsPositionOnGrid(cell_m3) && clues_.IsPositionOnGrid(cell_m2) && GetClue(cell_m3) == kClueWhite && GetClue(cell_m2) == kClueWhite) is_triple = true;

			if (is_triple) {
				DecideEdge(pos + dir_cross, kEdgeLine);
				DecideEdge(pos - dir_cross, kEdgeLine);
			}
		}
	}
}
std::ostream &operator<<(std::ostream &stream, Field &field)
{
	for (Y y(0); y <= 2 * (field.height() - 1); ++y) {
		for (X x(0); x <= 2 * (field.width() - 1); ++x) {
			if (y % 2 == 0 && x % 2 == 0) {
				Clue c = field.GetClue(CellPosition(y / 2, x / 2));
				if (c == kClueBlack) stream << "#";
				else if (c == kClueWhite) stream << "O";
				else stream << "+";
			} else if (y % 2 == 1 && x % 2 == 0) {
				Field::EdgeState e = field.GetEdge(LoopPosition(y, x));
				if (e == Field::kEdgeUndecided) stream << " ";
				else if (e == Field::kEdgeLine) stream << "|";
				else stream << "X";
			} else if (y % 2 == 0 && x % 2 == 1) {
				Field::EdgeState e = field.GetEdge(LoopPosition(y, x));
				if (e == Field::kEdgeUndecided) stream << "   ";
				else if (e == Field::kEdgeLine) stream << "---";
				else stream << " X ";
			} else stream << "   ";
		}
		stream << "\n";
	}
	return stream;
}
}
}
