#include "sl_database.h"

namespace penciloid
{
namespace slitherlink
{
const Direction Database::kNeighbor[12] = {
	Direction(-2, -1),
	Direction(-2, 1),
	Direction(-1, -2),
	Direction(-1, 0),
	Direction(-1, 2),
	Direction(0, -1),
	Direction(0, 1),
	Direction(1, -2),
	Direction(1, 0),
	Direction(1, 2),
	Direction(2, -1),
	Direction(2, 1),
};

Database::Database() : data_(nullptr)
{
}
Database::~Database()
{
	Release();
}
void Database::CreateDefault()
{
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
	}
}
void Database::Release()
{
	if (data_) delete[] data_;
}
unsigned int Database::PatternToId(int (&pattern)[12])
{
	unsigned int ret = 0;
	for (int i = 11; i >= 0; --i) {
		ret = ret * 3 + pattern[i];
	}
	return ret;
}
void Database::IdToPattern(unsigned int id, int (*pattern)[12])
{
	for (int i = 0; i < 12; ++i) {
		(*pattern)[i] = id % 3;
		id /= 3;
	}
}
}
}