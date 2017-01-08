#include "nl_problem.h"

#include <algorithm>

namespace penciloid
{
namespace numberlink
{
Problem::Problem() : grid_()
{
}
Problem::Problem(Y height, X width) : grid_(height, width, kNoClue)
{
}
Problem::Problem(Y height, X width, const char* clues[]) : grid_(height, width, kNoClue)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('1' <= clues[y][x] && clues[y][x] <= '9') {
				SetClue(CellPosition(y, x), Clue(clues[y][x] - '0'));
			} else if ('a' <= clues[y][x] && clues[y][x] <= 'z') {
				SetClue(CellPosition(y, x), Clue(clues[y][x] - 'a' + 10));
			}
		}
	}
}
}
}
