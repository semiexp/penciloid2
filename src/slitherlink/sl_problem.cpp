#include "sl_problem.h"

#include <algorithm>

namespace penciloid
{
namespace slitherlink
{
Problem::Problem() : field_clue_(nullptr), height_(0), width_(0)
{
}
Problem::Problem(Y height, X width) : field_clue_(nullptr), height_(height), width_(width)
{
	field_clue_ = new Clue[static_cast<int>(height) * static_cast<int>(width)];
}
Problem::Problem(Y height, X width, const char* clues[]) : field_clue_(nullptr), height_(height), width_(width)
{
	field_clue_ = new Clue[static_cast<int>(height) * static_cast<int>(width)];
	
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= clues[y][x] && clues[y][x] <= '3') {
				SetClue(Position(y, x), clues[y][x] - '0');
			}
		}
	}
}
Problem::Problem(const Problem &other) : field_clue_(nullptr), height_(other.height_), width_(other.width_)
{
	field_clue_ = new Clue[static_cast<int>(height()) * static_cast<int>(width())];
	std::copy(other.field_clue_, other.field_clue_ + static_cast<int>(height()) * static_cast<int>(width()), field_clue_);
}
Problem::Problem(Problem &&other) : field_clue_(other.field_clue_), height_(other.height_), width_(other.width_)
{
	other.field_clue_ = nullptr;
}
Problem::~Problem()
{
	if (field_clue_) delete[] field_clue_;
}
}
}
