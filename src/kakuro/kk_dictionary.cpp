#include "kk_dictionary.h"

namespace penciloid
{
namespace kakuro
{
Dictionary::Dictionary() : data_(nullptr)
{
}
Dictionary::~Dictionary()
{
	if (data_ != nullptr) delete[] data_;
}
void Dictionary::CreateDefault()
{
	Release();

	data_ = new unsigned int[kDictionarySize];

	for (int n_cells = 0; n_cells <= kMaxCellValue; ++n_cells) {
		for (int n_sum = 0; n_sum <= kMaxGroupSum; ++n_sum) {
			for (int bits = 0; bits < (1 << kMaxCellValue); ++bits) {
				int idx = GetIndex(n_cells, n_sum, bits);

				if (n_cells == 0) {
					data_[idx] = 0;
					continue;
				} else if (n_cells == 1) {
					if (1 <= n_sum && n_sum <= kMaxCellValue && (bits & (1 << (n_sum - 1)))) {
						data_[idx] = 1 << (n_sum - 1);
					} else {
						data_[idx] = 0;
					}
					continue;
				}

				data_[idx] = 0;
				for (int n = 1; n <= kMaxCellValue; ++n) if (bits & (1 << (n - 1))) {
					int cand = data_[GetIndex(n_cells - 1, n_sum - n, bits ^ (1 << (n - 1)))];
					if (cand != 0) {
						data_[idx] |= cand | (1 << (n - 1));
					}
				}
			}
		}
	}
}
void Dictionary::Release()
{
	if (data_) delete[] data_;
}
}
}
