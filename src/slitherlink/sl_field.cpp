#include "sl_field.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "sl_problem.h"
#include "sl_dictionary.h"

namespace penciloid
{
namespace slitherlink
{
Field::Field() : GridLoop<Field>(), field_clue_(), database_(nullptr), method_()
{
}
Field::Field(Y height, X width, Dictionary *database, const Method &met) :
	GridLoop<Field>(height, width), field_clue_(height, width, kNoClue), database_(database), method_(met)
{
	GridLoop<Field>::SetMethod(met.grid_loop_method);
}
Field::Field(const Field& other) : GridLoop<Field>(other), field_clue_(other.field_clue_), database_(other.database_), method_(other.method_)
{
	GridLoop<Field>::SetMethod(other.method_.grid_loop_method);
}
Field::Field(Field&& other) : GridLoop<Field>(other), field_clue_(std::move(other.field_clue_)), database_(other.database_), method_(other.method_)
{
	GridLoop<Field>::SetMethod(other.method_.grid_loop_method);
}
Field &Field::operator=(const Field& other)
{
	GridLoop<Field>::operator=(other);

	field_clue_ = other.field_clue_;
	database_ = other.database_;
	method_ = other.method_;

	return *this;
}
Field &Field::operator=(Field&& other)
{
	GridLoop<Field>::operator=(other);

	field_clue_ = std::move(other.field_clue_);
	database_ = other.database_;
	method_ = other.method_;

	return *this;
}
Field::Field(const Problem& problem, Dictionary *database, const Method &met) :
	GridLoop<Field>(problem.height(), problem.width()), field_clue_(problem.height(), problem.width(), kNoClue), database_(database), method_(met)
{
	GridLoop<Field>::SetMethod(met.grid_loop_method);
	int cell_count = static_cast<int>(height()) * static_cast<int>(width());
	auto clue_adder = [this, &problem]() {
		for (Y y(0); y < height(); ++y) {
			for (X x(0); x < width(); ++x) {
				if (problem.GetClue(CellPosition(y, x)) != kNoClue) AddClue(CellPosition(y, x), problem.GetClue(CellPosition(y, x)));
			}
		}
	};
	QueuedRun(clue_adder);
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

	field_clue_(pos) = clue;
	ApplyTheorem(LoopPosition(pos.y * 2 + 1, pos.x * 2 + 1));
	Check(LoopPosition(pos.y * 2 + 1, pos.x * 2 + 1));
}
void Field::Inspect(LoopPosition pos)
{
	if (!(pos.y % 2 == 1 && pos.x % 2 == 1)) return;
	if (GetClue(CellPosition(pos.y / 2, pos.x / 2)) == kNoClue) return;

	if (database_ != nullptr && method_.around_cell) {
		unsigned int db_id = 0;
		for (int i = 11; i >= 0; --i) {
			EdgeState status = GetEdgeSafe(pos + Dictionary::kNeighbor[i]);

			if (status == kEdgeUndecided) db_id = db_id * 3 + Dictionary::kUndecided;
			else if (status == kEdgeLine) db_id = db_id * 3 + Dictionary::kLine;
			else if (status == kEdgeBlank) db_id = db_id * 3 + Dictionary::kBlank;
		}

		Clue c = GetClue(CellPosition(pos.y / 2, pos.x / 2));
		unsigned int db_result = database_->Get(db_id, GetClue(CellPosition(pos.y / 2, pos.x / 2)));
		if (db_result == 0xffffffffU) {
			SetInconsistent();
			return;
		}

		for (int i = 0; i < 12; ++i) {
			int new_status = (db_result >> (2 * i)) & 3;
			if (new_status == Dictionary::kLine) {
				DecideEdge(pos + Dictionary::kNeighbor[i], kEdgeLine);
			}
			if (new_status == Dictionary::kBlank) {
				LoopPosition edge_pos = pos + Dictionary::kNeighbor[i];
				if (0 <= edge_pos.y && edge_pos.y <= 2 * height() && 0 <= edge_pos.x && edge_pos.x <= 2 * width()) {
					DecideEdge(pos + Dictionary::kNeighbor[i], kEdgeBlank);
				}
			}
		}
	}

	if (method_.diagonal_chain) CheckDiagonalChain(pos);
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
		if (method_.adjacent_3) {
			for (int i = 0; i < 4; ++i) {
				CellPosition pos2 = CellPosition(pos.y / 2, pos.x / 2) + dir[i];
				if (0 <= pos2.y && pos2.y < height() && 0 <= pos2.x && pos2.x < width() && GetClue(pos2) == 3) {
					DecideEdge(pos + dir[i], kEdgeLine);
					DecideEdge(pos + dir[i] + dir[i] + dir[i], kEdgeLine);
					DecideEdge(pos - dir[i], kEdgeLine);
					DecideEdge(pos + dir[i] + dir[i ^ 1] + dir[i ^ 1], kEdgeBlank);
					DecideEdge(pos + dir[i] - dir[i ^ 1] - dir[i ^ 1], kEdgeBlank);
				}
			}
		}
		
		// Diagonal 3s
		if (method_.diagonal_3) {
			for (int i = 0; i < 4; ++i) {
				CellPosition pos2 = CellPosition(pos.y / 2, pos.x / 2) + dir[i] + dir[(i + 1) & 3];
				if (0 <= pos2.y && pos2.y < height() && 0 <= pos2.x && pos2.x < width() && GetClue(pos2) == 3) {
					DecideEdge(pos - dir[i], kEdgeLine);
					DecideEdge(pos - dir[(i + 1) & 3], kEdgeLine);
					DecideEdge(pos + dir[i] + dir[i] + dir[i] + dir[(i + 1) & 3] + dir[(i + 1) & 3], kEdgeLine);
					DecideEdge(pos + dir[i] + dir[i] + dir[(i + 1) & 3] + dir[(i + 1) & 3] + dir[(i + 1) & 3], kEdgeLine);
				}
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
		if (s1 == kEdgeUndecided || s2 == kEdgeUndecided) return prev_parity;
		if (s1 == s2) return 0;
		return 1;
	};
	static const auto edge_state_by_parity = [](EdgeState s1, int parity) {
		if (parity == 1) {
			if (s1 == kEdgeLine) return kEdgeBlank;
			else return kEdgeLine;
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
					DecideEdge(cell_loop_pos - dir1, kEdgeBlank);
					DecideEdge(cell_loop_pos - dir2, kEdgeBlank);
				} else if (vertex_parity == 1) {
					DecideEdge(cell_loop_pos + dir1, kEdgeBlank);
					DecideEdge(cell_loop_pos + dir2, kEdgeBlank);
				}
				break;
			}
			if (GetClue(cell_pos) == 3) {
				if (vertex_parity == 0) {
					DecideEdge(cell_loop_pos - dir1, kEdgeLine);
					DecideEdge(cell_loop_pos - dir2, kEdgeLine);
				} else if (vertex_parity == 1) {
					DecideEdge(cell_loop_pos + dir1, kEdgeLine);
					DecideEdge(cell_loop_pos + dir2, kEdgeLine);
				}
				break;
			}
			if (GetClue(cell_pos) == 2) {
				if (GetEdgeSafe(cell_loop_pos + dir1) != kEdgeUndecided) {
					DecideEdge(cell_loop_pos + dir2, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir1), vertex_parity));
				}
				if (GetEdgeSafe(cell_loop_pos + dir2) != kEdgeUndecided) {
					DecideEdge(cell_loop_pos + dir1, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir2), vertex_parity));
				}
				if (GetEdgeSafe(cell_loop_pos + dir1 + dir2 * 2) != kEdgeUndecided) {
					DecideEdge(cell_loop_pos + dir2 + dir1 * 2, edge_state_by_parity(GetEdgeSafe(cell_loop_pos + dir1 + dir2 * 2), vertex_parity));
				}
				if (GetEdgeSafe(cell_loop_pos + dir2 + dir1 * 2) != kEdgeUndecided) {
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
				if (status == Field::kEdgeUndecided) stream << "   ";
				else if (status == Field::kEdgeLine) stream << "---";
				else if (status == Field::kEdgeBlank) stream << " X ";
			} else if (y % 2 == 1 && x % 2 == 0) {
				Field::EdgeState status = field.GetEdge(LoopPosition(y, x));
				if (status == Field::kEdgeUndecided) stream << " ";
				else if (status == Field::kEdgeLine) stream << "|";
				else if (status == Field::kEdgeBlank) stream << "X";
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
