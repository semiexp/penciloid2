#pragma once

#include "../common/type.h"

namespace penciloid
{
namespace slitherlink
{
class Database
{
public:
	static const unsigned int kUndecided = 0;
	static const unsigned int kLine = 1;
	static const unsigned int kBlank = 2;
	static const Direction kNeighbor[12];

	Database();
	~Database();

	void CreateDefault();
	void Release();

	// clue: 0, 1, 2 or 3
	// edge_pattern: \sum_{i=0}^11 ev[i] * (3^i)
	// where ev[i] = (kUndecided if the edge at (cell + kNeighbor[i]) is undecided, ...)
	// returns \sum_{i=0}^11 ev'[i] * (4^i) or 0xffffffff (if inconsistent)
	inline unsigned int Get(unsigned int edge_pattern, unsigned int clue) {
		return data_[edge_pattern + clue * kDatabaseSizeForEachClue];
	}

private:
	// 531441 = 3^12
	static const int kDatabaseSizeForEachClue = 531441;
	static const int kDatabaseSize = kDatabaseSizeForEachClue * 4;

	unsigned int PatternToId(int (&pattern)[12]);
	void IdToPattern(unsigned int id, int (*pattern)[12]);
	inline unsigned int CountLines(int (&pattern)[12], int p1, int p2, int p3, int p4) {
		return
			  (pattern[p1] == kLine ? 1 : 0) + (pattern[p2] == kLine ? 1 : 0)
			+ (pattern[p3] == kLine ? 1 : 0) + (pattern[p4] == kLine ? 1 : 0);
	}
	inline bool IsValidVertex(int (&pattern)[12], int p1, int p2, int p3, int p4) {
		int count = CountLines(pattern, p1, p2, p3, p4);
		return count == 0 || count == 2;
	}

	unsigned int *data_;
};
}
}
