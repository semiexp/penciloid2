#pragma once

#include "../common/type.h"
#include "../common/grid.h"
#include "sl_dictionary_method.h"

namespace penciloid
{
namespace slitherlink
{
class Dictionary
{
public:
	static const unsigned int kUndecided = 0;
	static const unsigned int kLine = 1;
	static const unsigned int kBlank = 2;
	static const Direction kNeighbor[12];

	Dictionary();

	Dictionary(const Dictionary &) = delete;
	Dictionary(Dictionary &&) = delete;
	Dictionary &operator=(const Dictionary &) = delete;
	Dictionary &operator=(Dictionary&&) = delete;

	~Dictionary();
	
	void CreateDefault();
	void CreateRestricted(const DictionaryMethod &method);
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

	// Apply methods to <field>.
	// Returns true if and only if inconsistency is detected.
	bool ApplyTwoLines(const DictionaryMethod &method, Grid<int> &field, int clue);
	bool ApplyAdjacentLines(const DictionaryMethod &method, Grid<int> &field, int clue);
	bool ApplyCornerClue(const DictionaryMethod &method, Grid<int> &field, int clue);
	bool ApplyLineToClue(const DictionaryMethod &method, Grid<int> &field, int clue);
	bool ApplyPartialLineToClue(const DictionaryMethod &method, Grid<int> &field, int clue);
	bool ApplyLineFromClue(const DictionaryMethod &method, Grid<int> &field, int clue);

	unsigned int *data_;
};
}
}
