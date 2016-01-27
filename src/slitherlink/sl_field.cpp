#include "sl_field.h"

#include <algorithm>
#include <iostream>

#include "sl_database.h"

namespace penciloid
{
namespace slitherlink
{
Field::Field() : GridLoop<Field>(), field_clue_(nullptr), database_(nullptr)
{
}
Field::Field(Y height, X width) : GridLoop<Field>(height, width), field_clue_(nullptr), database_(nullptr)
{
	field_clue_ = new Clue[height * width];
	std::fill(field_clue_, field_clue_ + height * width, kNoClue);
}
Field::Field(const Field& other) : GridLoop<Field>(other), field_clue_(nullptr), database_(other.database_)
{
	field_clue_ = new Clue[height() * width()];
	memcpy(field_clue_, other.field_clue_, height() * width() * sizeof(Clue));
}
Field::~Field()
{
	if (field_clue_) delete[] field_clue_;
}
void Field::AddClue(Position pos, Clue clue)
{
	if (GetClue(pos) != kNoClue) {
		if (GetClue(pos) != clue) {
			SetInconsistent();
		}
		return;
	}

	field_clue_[CellId(pos)] = clue;
	Check(Position(pos.y * 2 + 1, pos.x * 2 + 1));
}
void Field::Inspect(Position pos)
{
	if (!(pos.y % 2 == 1 && pos.x % 2 == 1)) return;
	if (GetClue(Position(pos.y / 2, pos.x / 2)) == kNoClue) return;

	unsigned int db_id = 0;
	for (int i = 11; i >= 0; --i) {
		EdgeState status = GetEdgeSafe(pos + Database::kNeighbor[i]);
		
		if (status == EDGE_UNDECIDED) db_id = db_id * 3 + Database::kUndecided;
		else if (status == EDGE_LINE) db_id = db_id * 3 + Database::kLine;
		else if (status == EDGE_BLANK) db_id = db_id * 3 + Database::kBlank;
	}

	Clue c = GetClue(Position(pos.y / 2, pos.x / 2));
	unsigned int db_result = database_->Get(db_id, GetClue(Position(pos.y / 2, pos.x / 2)));
	if (db_result == 0xffffffffU) {
		SetInconsistent();
		return;
	}

	for (int i = 0; i < 12; ++i) {
		int new_status = (db_result >> (2 * i)) & 3;
		if (new_status == Database::kLine) DecideEdge(pos + Database::kNeighbor[i], EDGE_LINE);
		if (new_status == Database::kBlank) {
			Position edge_pos = pos + Database::kNeighbor[i];
			if (0 <= edge_pos.y && edge_pos.y <= 2 * height() && 0 <= edge_pos.x && edge_pos.x <= 2 * width()) {
				DecideEdge(pos + Database::kNeighbor[i], EDGE_BLANK);
			}
		}
	}
}
std::ostream& operator<<(std::ostream &stream, Field &field)
{
	for (Y y = 0; y <= 2 * field.height(); ++y) {
		for (X x = 0; x <= 2 * field.width(); ++x) {
			if (y % 2 == 0 && x % 2 == 0) {
				stream << "+";
			} else if (y % 2 == 0 && x % 2 == 1) {
				Field::EdgeState status = field.GetEdge(Position(y, x));
				if (status == Field::EDGE_UNDECIDED) stream << "   ";
				else if (status == Field::EDGE_LINE) stream << "---";
				else if (status == Field::EDGE_BLANK) stream << " X ";
			} else if (y % 2 == 1 && x % 2 == 0) {
				Field::EdgeState status = field.GetEdge(Position(y, x));
				if (status == Field::EDGE_UNDECIDED) stream << " ";
				else if (status == Field::EDGE_LINE) stream << "|";
				else if (status == Field::EDGE_BLANK) stream << "X";
			} else if (y % 2 == 1 && x % 2 == 1) {
				Field::Clue clue = field.GetClue(Position(y / 2, x / 2));
				if (clue == Field::kNoClue) stream << "   ";
				else stream << " " << clue << " ";
			}
		}
		stream << "\n";
	}
	return stream;
}
}
}
