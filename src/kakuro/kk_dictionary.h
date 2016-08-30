#pragma once

#include "../common/type.h"

namespace penciloid
{
namespace kakuro
{
class Dictionary
{
public:
	Dictionary();

	Dictionary(const Dictionary &) = delete;
	Dictionary(Dictionary &&) = delete;
	Dictionary &operator=(const Dictionary &) = delete;
	Dictionary &operator=(Dictionary&&) = delete;

	~Dictionary();

	void CreateDefault();
	void Release();

	// n_cells: 0, 1, 2, ..., 9
	// n_sum: 0, 1, 2, ..., 45
	// bits_available_num: 0, 1, ..., (1 << 9) - 1
	inline unsigned int GetPossibleCandidates(int n_cells, int n_sum, int bits_available_num) {
		return data_[GetIndex(n_cells, n_sum, bits_available_num)];
	}

private:
	static const int kMaxCellValue = 9;
	static const int kMaxGroupSum = kMaxCellValue * (kMaxCellValue + 1) / 2;
	static const int kDictionarySize = (kMaxCellValue + 1) * (kMaxGroupSum + 1) * (1 << kMaxCellValue);

	inline int GetIndex(int n_cells, int n_sum, int bits_available_num) {
		return ((n_cells * (1 + kMaxGroupSum) + n_sum) << kMaxCellValue) | bits_available_num;
	}

	unsigned int *data_;
};
}
}
