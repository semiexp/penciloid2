#include "sl_field.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "sl_problem.h"
#include "sl_database.h"

namespace penciloid
{
namespace slitherlink
{
Field::Field() : GridLoop<Field>(), field_clue_(), database_(nullptr)
{
}
Field::Field(Y height, X width, Database *database) : GridLoop<Field>(height, width), field_clue_(height, width, kNoClue), database_(database)
{
}
Field::Field(const Field& other) : GridLoop<Field>(other), field_clue_(other.field_clue_), database_(other.database_)
{
}
Field::Field(Field&& other) : GridLoop<Field>(other), field_clue_(std::move(other.field_clue_)), database_(other.database_)
{
}
Field &Field::operator=(const Field& other)
{
	GridLoop<Field>::operator=(other);

	field_clue_ = other.field_clue_;
	database_ = other.database_;

	return *this;
}
Field &Field::operator=(Field&& other)
{
	GridLoop<Field>::operator=(other);

	field_clue_ = std::move(other.field_clue_);
	database_ = other.database_;

	return *this;
}
Field::Field(const Problem& problem, Database *database) : GridLoop<Field>(problem.height(), problem.width()), field_clue_(problem.height(), problem.width(), kNoClue), database_(database)
{
	int cell_count = static_cast<int>(height()) * static_cast<int>(width());
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			if (problem.GetClue(CellPosition(y, x)) != kNoClue) AddClue(CellPosition(y, x), problem.GetClue(CellPosition(y, x)));
		}
	}
}
Field::~Field()
{
}
void Field::AddClue(CellPosition pos, Clue clue)
{
	if (GetClue(pos) != kNoClue) {
		if (GetClue(pos) != clue) {
			SetInconsistent();
		}
		return;
	}

	field_clue_.at(pos) = clue;
	ApplyTheorem(LoopPosition(pos.y * 2 + 1, pos.x * 2 + 1));
	Check(LoopPosition(pos.y * 2 + 1, pos.x * 2 + 1));
}
void Field::Inspect(LoopPosition pos)
{
	if (database_ == nullptr) return;
	if (!(pos.y % 2 == 1 && pos.x % 2 == 1)) return;
	if (GetClue(CellPosition(pos.y / 2, pos.x / 2)) == kNoClue) return;

	unsigned int db_id = 0;
	for (int i = 11; i >= 0; --i) {
		EdgeState status = GetEdgeSafe(pos + Database::kNeighbor[i]);
		
		if (status == EDGE_UNDECIDED) db_id = db_id * 3 + Database::kUndecided;
		else if (status == EDGE_LINE) db_id = db_id * 3 + Database::kLine;
		else if (status == EDGE_BLANK) db_id = db_id * 3 + Database::kBlank;
	}

	Clue c = GetClue(CellPosition(pos.y / 2, pos.x / 2));
	unsigned int db_result = database_->Get(db_id, GetClue(CellPosition(pos.y / 2, pos.x / 2)));
	if (db_result == 0xffffffffU) {
		SetInconsistent();
		return;
	}

	for (int i = 0; i < 12; ++i) {
		int new_status = (db_result >> (2 * i)) & 3;
		if (new_status == Database::kLine) {
			DecideEdge(pos + Database::kNeighbor[i], EDGE_LINE);
		}
		if (new_status == Database::kBlank) {
			LoopPosition edge_pos = pos + Database::kNeighbor[i];
			if (0 <= edge_pos.y && edge_pos.y <= 2 * height() && 0 <= edge_pos.x && edge_pos.x <= 2 * width()) {
				DecideEdge(pos + Database::kNeighbor[i], EDGE_BLANK);
			}
		}
	}

	CheckDiagonalChain(pos);
}
void Field::ApplyTheorem(LoopPosition pos)
{
	// pos: coordinate of GridLoop
	static const Direction dir[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	if (GetClue(CellPosition(pos.y / 2, pos.x / 2)) == 3) {
		// Adjacent 3s
		for (int i = 0; i < 4; ++i) {
			CellPosition pos2 = CellPosition(pos.y / 2, pos.x / 2) + dir[i];
			if (0 <= pos2.y && pos2.y < height() && 0 <= pos2.x && pos2.x < width() && GetClue(pos2) == 3) {
				DecideEdge(pos + dir[i], EDGE_LINE);
				DecideEdge(pos + dir[i] + dir[i] + dir[i], EDGE_LINE);
				DecideEdge(pos - dir[i], EDGE_LINE);
				DecideEdge(pos + dir[i] + dir[i ^ 1] + dir[i ^ 1], EDGE_BLANK);
				DecideEdge(pos + dir[i] - dir[i ^ 1] - dir[i ^ 1], EDGE_BLANK);
			}
		}

		// Diagonal 3s
		for (int i = 0; i < 4; ++i) {
			CellPosition pos2 = CellPosition(pos.y / 2, pos.x / 2) + dir[i] + dir[(i + 1) & 3];
			if (0 <= pos2.y && pos2.y < height() && 0 <= pos2.x && pos2.x < width() && GetClue(pos2) == 3) {
				DecideEdge(pos - dir[i], EDGE_LINE);
				DecideEdge(pos - dir[(i + 1) & 3], EDGE_LINE);
				DecideEdge(pos + dir[i] + dir[i] + dir[i] + dir[(i + 1) & 3] + dir[(i + 1) & 3], EDGE_LINE);
				DecideEdge(pos + dir[i] + dir[i] + dir[(i + 1) & 3] + dir[(i + 1) & 3] + dir[(i + 1) & 3], EDGE_LINE);
			}
		}
	}
}
void Field::CheckDiagonalChain(LoopPosition pos)
{
	static const Direction kDirs[] = {
		Direction(Y(1), X(0)),
		Direction(Y(0), X(1)),
		Direction(Y(-1), X(0)),
		Direction(Y(0), X(-1))
	};

	if (GetClue(CellPosition(pos.y / 2, pos.x / 2)) == kNoClue) return;

	static const auto check_and_update_parity = [](EdgeState s1, EdgeState s2, int prev_parity) {
		if (s1 == EDGE_UNDECIDED || s2 == EDGE_UNDECIDED) return prev_parity;
		if (s1 == s2) return 0;
		return 1;
	};
	static const auto edge_state_by_parity = [](EdgeState s1, int parity) {
		if (parity == 1) {
			if (s1 == EDGE_LINE) return EDGE_BLANK;
			else return EDGE_LINE;
		}
		return s1;
	};

	for (int d = 0; d < 4; ++d) {
		Direction dir1 = kDirs[d], dir2 = kDirs[(d + 1) % 4];
		LoopPosition cell_loop_pos = pos;
		CellPosition cell_pos(pos.y / 2, pos.x / 2);

		int vertex_parity = -1;
		vertex_parity = check_and_update_parity(
			GetEdgeSafe(cell_loop_pos - dir1),
			GetEdgeSafe(cell_loop_pos - dir2),
			vertex_parity
			);
		vertex_parity = check_and_update_parity(
			GetEdgeSafe(cell_loop_pos - dir1 - dir2 * 2),
			GetEdgeSafe(cell_loop_pos - dir2 - dir1 * 2),
			vertex_parity
			);

		if (vertex_parity == -1) continue;

		while (0 <= cell_pos.y && cell_pos.y < height() && 0 <= cell_pos.x && cell_pos.x < width()) {
			if (GetClue(cell_pos) == kNoClue || GetClue(cell_pos) == 0) break;

			if (GetClue(cell_pos) == 1) {
				if (vertex_parity == 0) {
					DecideEdge(cell_loop_pos - dir1, EDGE_BLANK);
					DecideEdge(cell_loop_pos - dir2, EDGE_BLANK);
				} else if (vertex_parity == 1) {
					DecideEdge(cell_loop_pos + dir1, EDGE_BLANK);
					DecideEdge(cell_loop_pos + dir2, EDGE_BLANK);
				}
				break;
			}
			if (GetClue(cell_pos) == 3) {
				if (vertex_parity == 0) {
					DecideEdge(cell_loop_pos - dir1, EDGE_LINE);
					DecideEdge(cell_loop_pos - dir2, EDGE_LINE);
				} else if (vertex_parity == 1) {
					DecideEdge(cell_loop_pos + dir1, EDGE_LINE);
					DecideEdge(cell_loop_pos + dir2, EDGE_LINE);
				}
				break;
			}
			if (GetClue(cell_pos) == 2) {
				if (GetEdgeSafe(cell_loop_pos + dir1) != EDGE_UNDECIDED) {
					DecideEdge(cell_loop_pos + dir2, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir1), vertex_parity));
				}
				if (GetEdgeSafe(cell_loop_pos + dir2) != EDGE_UNDECIDED) {
					DecideEdge(cell_loop_pos + dir1, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir2), vertex_parity));
				}
				if (GetEdgeSafe(cell_loop_pos + dir1 + dir2 * 2) != EDGE_UNDECIDED) {
					DecideEdge(cell_loop_pos + dir2 + dir1 * 2, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir1 + dir2 * 2), vertex_parity));
				}
				if (GetEdgeSafe(cell_loop_pos + dir2 + dir1 * 2) != EDGE_UNDECIDED) {
					DecideEdge(cell_loop_pos + dir1 + dir2 * 2, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir2 + dir1 * 2), vertex_parity));
				}
			}

			cell_pos = cell_pos + dir1 + dir2;
			cell_loop_pos = cell_loop_pos + (dir1 + dir2) * 2;
		}
	}
}
std::ostream& operator<<(std::ostream &stream, Field &field)
{
	for (Y y(0); y <= 2 * field.height(); ++y) {
		for (X x(0); x <= 2 * field.width(); ++x) {
			if (y % 2 == 0 && x % 2 == 0) {
				stream << "+";
			} else if (y % 2 == 0 && x % 2 == 1) {
				Field::EdgeState status = field.GetEdge(LoopPosition(y, x));
				if (status == Field::EDGE_UNDECIDED) stream << "   ";
				else if (status == Field::EDGE_LINE) stream << "---";
				else if (status == Field::EDGE_BLANK) stream << " X ";
			} else if (y % 2 == 1 && x % 2 == 0) {
				Field::EdgeState status = field.GetEdge(LoopPosition(y, x));
				if (status == Field::EDGE_UNDECIDED) stream << " ";
				else if (status == Field::EDGE_LINE) stream << "|";
				else if (status == Field::EDGE_BLANK) stream << "X";
			} else if (y % 2 == 1 && x % 2 == 1) {
				Clue clue = field.GetClue(CellPosition(y / 2, x / 2));
				if (clue == kNoClue) stream << "   ";
				else stream << " " << clue << " ";
			}
		}
		stream << "\n";
	}
	return stream;
}
}
}
