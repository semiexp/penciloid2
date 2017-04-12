#include "kk_answer.h"

#include <utility>

namespace penciloid
{
namespace kakuro
{
Answer::Answer() : grid_()
{
}
Answer::Answer(Y height, X width) : grid_(height, width, kNoClueValue)
{
}
Answer::Answer(const Answer &other) : grid_(other.grid_)
{
}
Answer::Answer(Answer &&other) : grid_(std::move(other.grid_))
{
}
Answer &Answer::operator=(const Answer &other)
{
	grid_ = other.grid_;
	return *this;
}
Answer &Answer::operator=(Answer &&other)
{
	grid_ = std::move(other.grid_);
	return *this;
}
Problem Answer::ExtractProblem() const
{
	Problem ret(height(), width());
	for (Y y(0); y < height(); ++y) {
		for (X x(0); x < width(); ++x) {
			int val = GetValue(CellPosition(y, x));
			if (val == -1) {
				int vertical = 0;
				for (Y y2(y); y2 < height(); ++y2) {
					if (GetValue(CellPosition(y2, x)) == kNoClueValue) {
						break;
					} else {
						vertical += GetValue(CellPosition(y2, x));
					}
				}
				int horizontal = 0;
				for (X x2(x); x2 < width(); ++x2) {
					if (GetValue(CellPosition(y, x2)) == kNoClueValue) {
						break;
					} else {
						horizontal += GetValue(CellPosition(y, x2));
					}
				}

				ret.SetClue(CellPosition(y, x), Clue(
					vertical != 0 ? vertical : kNoClueValue,
					horizontal != 0 ? horizontal : kNoClueValue
				));
			} else {
				ret.SetClue(CellPosition(y, x), kEmptyCell);
			}
		}
	}
	return ret;
}
}
}