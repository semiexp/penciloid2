#include "sl_dictionary.h"
#include "../common/grid.h"

namespace penciloid
{
namespace slitherlink
{
namespace
{
const CellPosition kCenter{ Y(2), X(2) };
}
const Direction Dictionary::kNeighbor[12] = {
	Direction(Y(-2), X(-1)),
	Direction(Y(-2), X(1)),
	Direction(Y(-1), X(-2)),
	Direction(Y(-1), X(0)),
	Direction(Y(-1), X(2)),
	Direction(Y(0), X(-1)),
	Direction(Y(0), X(1)),
	Direction(Y(1), X(-2)),
	Direction(Y(1), X(0)),
	Direction(Y(1), X(2)),
	Direction(Y(2), X(-1)),
	Direction(Y(2), X(1)),
};

Dictionary::Dictionary() : data_(nullptr)
{
}
Dictionary::~Dictionary()
{
	Release();
}
void Dictionary::CreateDefault()
{
	Release();

	data_ = new unsigned int[kDatabaseSize];

	for (int clue = 0; clue < 4; ++clue) {
		unsigned int offset = clue * kDatabaseSizeForEachClue;
		for (unsigned int id = kDatabaseSizeForEachClue; id--;){
			int pattern[12];
			int undecided_pos = -1;

			IdToPattern(id, &pattern);

			for (int i = 0; i < 12; ++i) {
				if (pattern[i] == kUndecided) undecided_pos = i;
			}

			if (undecided_pos == -1) {
				bool is_valid = true;
				if (!IsValidVertex(pattern, 0, 2, 3, 5)) is_valid = false;
				if (!IsValidVertex(pattern, 1, 3, 4, 6)) is_valid = false;
				if (!IsValidVertex(pattern, 5, 7, 8, 10)) is_valid = false;
				if (!IsValidVertex(pattern, 6, 8, 9, 11)) is_valid = false;
				if (CountLines(pattern, 3, 5, 6, 8) != clue) is_valid = false;

				if (is_valid) {
					data_[offset + id] = 0;
					for (int i = 0; i < 12; ++i) {
						data_[offset + id] |= pattern[i] << (2 * i);
					}
				} else data_[offset + id] = 0xffffffffU;
			} else {
				pattern[undecided_pos] = kLine;
				unsigned int ref_id1 = PatternToId(pattern);
				pattern[undecided_pos] = kBlank;
				unsigned int ref_id2 = PatternToId(pattern);

				data_[offset + id] = data_[offset + ref_id1] & data_[offset + ref_id2];
			}
		}

		for (unsigned int id = 0; id < kDatabaseSizeForEachClue; ++id){
			if (data_[offset + id] == 0xffffffffU) continue;

			int pattern[12];

			IdToPattern(id, &pattern);
			for (int i = 0; i < 12; ++i) {
				if (pattern[i] != kUndecided) data_[offset + id] &= ~(3 << (2 * i));
			}
		}
	}
}
void Dictionary::CreateRestricted(const DictionaryMethod &method)
{
	Release();

	data_ = new unsigned int[kDatabaseSize];

	for (int clue = 0; clue < 4; ++clue) {
		unsigned int offset = clue * kDatabaseSizeForEachClue;
		for (unsigned int id = 0; id < kDatabaseSizeForEachClue; ++id) {
			int pattern[12];
			Grid<int> field(Y(5), X(5), 0);

			IdToPattern(id, &pattern);
			for (int i = 0; i < 12; ++i) {
				field.at(CellPosition(Y(2), X(2)) + kNeighbor[i]) = pattern[i];
			}

			bool inconsistent = false;
			if (ApplyAdjacentLines(method, field, clue)) goto fail;
			if (ApplyTwoLines(method, field, clue)) goto fail;
			if (ApplyCornerClue(method, field, clue)) goto fail;
			if (ApplyLineToClue(method, field, clue)) goto fail;
			if (ApplyLineFromClue(method, field, clue)) goto fail;
			if (ApplyPartialLineToClue(method, field, clue)) goto fail;
			if (ApplyAdjacentLines(method, field, clue)) goto fail;
			if (ApplyTwoLines(method, field, clue)) goto fail;

			for (Y y(0); y < 5; ++y) {
				for (X x(0); x < 5; ++x) {
					if (static_cast<int>(y % 2) != static_cast<int>(x % 2) && field.at(CellPosition(y, x)) == 3) {
						goto fail;
					}
				}
			}
			if (inconsistent) {
				data_[offset + id] = 0xffffffffU;
				continue;
			}
			for (int i = 0; i < 12; ++i) {
				pattern[i] = field.at(kCenter + kNeighbor[i]);
			}
			data_[offset + id] = 0;
			for (int i = 0; i < 12; ++i) {
				data_[offset + id] |= pattern[i] << (2 * i);
			}

			IdToPattern(id, &pattern);
			for (int i = 0; i < 12; ++i) {
				if (pattern[i] != kUndecided) data_[offset + id] &= ~(3 << (2 * i));
			}
			continue;

		fail:
			data_[offset + id] = 0xffffffffU;
		}
	}
}
bool Dictionary::ApplyTwoLines(const DictionaryMethod &method, Grid<int> &field, int clue)
{
	for (Y y : {Y(1), Y(3)}) {
		for (X x : {X(1), X(3)}) {
			CellPosition center(y, x);
			int n_lines = 0, n_blanks = 0;
			
			for (Direction d : k4Neighborhood) {
				if (field.at(center + d) == kLine) ++n_lines;
				if (field.at(center + d) == kBlank) ++n_blanks;
			}

			if (n_lines >= 3) return true;
			if (n_lines == 1 && n_blanks == 3) return true;
			if (n_lines == 2 || n_blanks == 3) {
				// undecided edges -> blank edges
				for (Direction d : k4Neighborhood) {
					if (field.at(center + d) == kUndecided) field.at(center + d) |= kBlank;
				}
			}
			if (n_lines == 1 && n_blanks == 2) {
				// undecided edge -> line edge
				for (Direction d : k4Neighborhood) {
					if (field.at(center + d) == kUndecided) field.at(center + d) |= kLine;
				}
			}
		}
	}
	return false;
}
bool Dictionary::ApplyAdjacentLines(const DictionaryMethod &method, Grid<int> &field, int clue)
{
	if (!method.adjacent_lines) return false;

	int n_lines = 0, n_blanks = 0;
	for (int i = 0; i < 4; ++i) {
		int edge = field.at(kCenter + k4Neighborhood[i]);
		if (edge == kLine) ++n_lines;
		else if (edge == kBlank) ++n_blanks;
	}

	if (n_lines > clue || n_blanks > 4 - clue) return true;

	if (n_lines == clue) {
		for (int i = 0; i < 4; ++i) {
			CellPosition pos = kCenter + k4Neighborhood[i];
			if (field.at(pos) == kUndecided) field.at(pos) |= kBlank;
		}
	} else if (n_blanks == 4 - clue) {
		for (int i = 0; i < 4; ++i) {
			CellPosition pos = kCenter + k4Neighborhood[i];
			if (field.at(pos) == kUndecided) field.at(pos) |= kLine;
		}
	}
	return false;
}
bool Dictionary::ApplyCornerClue(const DictionaryMethod &method, Grid<int> &field, int clue)
{
	for (int d = 0; d < 4; ++d) {
		Direction d1 = k4Neighborhood[d], d2 = k4Neighborhood[(d + 1) % 4];
		if (!(field.at(kCenter + d1 + d2 * 2) == kBlank && field.at(kCenter + d1 * 2 + d2) == kBlank)) continue;

		if (clue == 1 && method.corner_clue_1) {
			field.at(kCenter + d1) |= kBlank;
			field.at(kCenter + d2) |= kBlank;
		}
		if (clue == 2 && method.corner_clue_2) {
			if (field.at(kCenter - d1) == kBlank
			 || field.at(kCenter - d2) == kBlank
			 || field.at(kCenter - d1 - d2 * 2) == kLine
			 || field.at(kCenter - d1 * 2 - d2) == kLine) {
				field.at(kCenter - d1) |= kBlank;
				field.at(kCenter - d2) |= kBlank;
				field.at(kCenter + d1) |= kLine;
				field.at(kCenter + d2) |= kLine;
			}
			if (field.at(kCenter - d1) == kLine
			 || field.at(kCenter - d2) == kLine) {
				field.at(kCenter - d1) |= kLine;
				field.at(kCenter - d2) |= kLine;
				field.at(kCenter + d1) |= kBlank;
				field.at(kCenter + d2) |= kBlank;
			}
			if (field.at(kCenter - d1 - d2 * 2) == kBlank
			 || field.at(kCenter - d1 * 2 - d2) == kBlank) {
				field.at(kCenter - d1 - d2 * 2) |= kBlank;
				field.at(kCenter - d1 * 2 - d2) |= kBlank;
			}
		}
		if (clue == 2 && method.corner_clue_2_hard) {
			for (int sgn : {-1, 1}) {
				if (field.at(kCenter - sgn * (d1 - d2 * 2)) == kLine
				 || field.at(kCenter - sgn * (d1 * 2 - d2)) == kBlank) {
					field.at(kCenter - sgn * (d1 - d2 * 2)) |= kLine;
					field.at(kCenter - sgn * (d1 * 2 - d2)) |= kBlank;
				}
				if (field.at(kCenter - sgn * (d1 - d2 * 2)) == kBlank
				 || field.at(kCenter - sgn * (d1 * 2 - d2)) == kLine) {
					field.at(kCenter - sgn * (d1 - d2 * 2)) |= kBlank;
					field.at(kCenter - sgn * (d1 * 2 - d2)) |= kLine;
				}
			}
		}
		if (clue == 3 && method.corner_clue_3) {
			field.at(kCenter + d1) |= kLine;
			field.at(kCenter + d2) |= kLine;
		}
	}
	return false;
}
bool Dictionary::ApplyLineToClue(const DictionaryMethod &method, Grid<int> &field, int clue)
{
	for (int d = 0; d < 4; ++d) {
		Direction d1 = k4Neighborhood[d], d2 = k4Neighborhood[(d + 1) % 4];

		if (clue == 3 && method.line_to_clue_3) {
			if (field.at(kCenter + d1 + d2 * 2) == kLine) {
				field.at(kCenter + d1 * 2 + d2) |= kBlank;
				field.at(kCenter - d1) |= kLine;
				field.at(kCenter - d2) |= kLine;
			}
			if (field.at(kCenter + d1 * 2 + d2) == kLine) {
				field.at(kCenter + d1 + d2 * 2) |= kBlank;
				field.at(kCenter - d1) |= kLine;
				field.at(kCenter - d2) |= kLine;
			}
		}
		if ((field.at(kCenter + d1 + d2 * 2) == kLine && field.at(kCenter + d1 * 2 + d2) == kBlank)
		 || (field.at(kCenter + d1 + d2 * 2) == kBlank && field.at(kCenter + d1 * 2 + d2) == kLine)) {
			if (clue == 1 && method.line_to_clue_1) {
				field.at(kCenter - d1) |= kBlank;
				field.at(kCenter - d2) |= kBlank;
			}
		}
		if ((field.at(kCenter + d1 + d2 * 2) == kLine && field.at(kCenter + d1 * 2 + d2) == kBlank)
		 || (field.at(kCenter + d1 + d2 * 2) == kBlank && field.at(kCenter + d1 * 2 + d2) == kLine)
		 || (field.at(kCenter + d1) == kLine && field.at(kCenter + d2) == kBlank)
		 || (field.at(kCenter + d1) == kBlank && field.at(kCenter + d2) == kLine)) {
			if (clue == 2 && method.line_to_clue_2) {
				if (field.at(kCenter - d1) == kLine) field.at(kCenter - d2) |= kBlank;
				if (field.at(kCenter - d1) == kBlank) field.at(kCenter - d2) |= kLine;
				if (field.at(kCenter - d2) == kLine) field.at(kCenter - d1) |= kBlank;
				if (field.at(kCenter - d2) == kBlank) field.at(kCenter - d1) |= kLine;
				if (field.at(kCenter - d1 - d2 * 2) == kLine) field.at(kCenter - d2 - d1 * 2) |= kBlank;
				if (field.at(kCenter - d1 - d2 * 2) == kBlank) field.at(kCenter - d2 - d1 * 2) |= kLine;
				if (field.at(kCenter - d2 - d1 * 2) == kLine) field.at(kCenter - d1 - d2 * 2) |= kBlank;
				if (field.at(kCenter - d2 - d1 * 2) == kBlank) field.at(kCenter - d1 - d2 * 2) |= kLine;
			}
		}
	}
	return false;
}
bool Dictionary::ApplyPartialLineToClue(const DictionaryMethod &method, Grid<int> &field, int clue)
{
	for (int d = 0; d < 4; ++d) {
		Direction d1 = k4Neighborhood[d], d2 = k4Neighborhood[(d + 1) % 4];

		if (clue == 2 && method.partial_line_to_clue_2) {
			if ((field.at(kCenter + d1 + d2 * 2) == kLine || field.at(kCenter + d1 * 2 + d2) == kLine)
			 && (field.at(kCenter - d1) == kBlank || field.at(kCenter - d2) == kBlank)) {
				if (field.at(kCenter + d1 + d2 * 2) == kLine) field.at(kCenter + d1 * 2 + d2) |= kBlank;
				if (field.at(kCenter + d1 * 2 + d2) == kLine) field.at(kCenter + d1 + d2 * 2) |= kBlank;
				if (field.at(kCenter - d1) == kBlank) field.at(kCenter - d2) |= kLine;
				if (field.at(kCenter - d2) == kBlank) field.at(kCenter - d1) |= kLine;
			}
			if ((field.at(kCenter + d1 + d2 * 2) == kLine || field.at(kCenter + d1 * 2 + d2) == kLine)
			 && (field.at(kCenter - d1 - d2 * 2) == kLine || field.at(kCenter - d1 * 2 - d2) == kLine)) {
				if (field.at(kCenter + d1 + d2 * 2) == kLine) field.at(kCenter + d1 * 2 + d2) |= kBlank;
				if (field.at(kCenter + d1 * 2 + d2) == kLine) field.at(kCenter + d1 + d2 * 2) |= kBlank;
				if (field.at(kCenter - d1 - d2 * 2) == kLine) field.at(kCenter - d1 * 2 - d2) |= kBlank;
				if (field.at(kCenter - d1 * 2 - d2) == kLine) field.at(kCenter - d1 - d2 * 2) |= kBlank;
			}
		}
	}
	return false;
}
bool Dictionary::ApplyLineFromClue(const DictionaryMethod &method, Grid<int> &field, int clue)
{
	for (int d = 0; d < 4; ++d) {
		Direction d1 = k4Neighborhood[d], d2 = k4Neighborhood[(d + 1) % 4];

		if ((clue == 1 && method.line_from_clue_1 && field.at(kCenter + d1) == kBlank && field.at(kCenter + d2) == kBlank)
		 || (clue == 3 && method.line_from_clue_3 && field.at(kCenter + d1) == kLine && field.at(kCenter + d2) == kLine)) {
			if (field.at(kCenter - d1 - d2 * 2) == kLine) field.at(kCenter - d1 * 2 - d2) |= kBlank;
			if (field.at(kCenter - d1 - d2 * 2) == kBlank) field.at(kCenter - d1 * 2 - d2) |= kLine;
			if (field.at(kCenter - d2 - d1 * 2) == kLine) field.at(kCenter - d2 * 2 - d1) |= kBlank;
			if (field.at(kCenter - d2 - d1 * 2) == kBlank) field.at(kCenter - d2 * 2 - d1) |= kLine;
		}
	}
	return false;
}
void Dictionary::Release()
{
	if (data_) delete[] data_;
}
unsigned int Dictionary::PatternToId(int (&pattern)[12])
{
	unsigned int ret = 0;
	for (int i = 11; i >= 0; --i) {
		ret = ret * 3 + pattern[i];
	}
	return ret;
}
void Dictionary::IdToPattern(unsigned int id, int (*pattern)[12])
{
	for (int i = 0; i < 12; ++i) {
		(*pattern)[i] = id % 3;
		id /= 3;
	}
}
}
}
