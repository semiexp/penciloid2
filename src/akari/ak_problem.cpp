#include "ak_problem.h"

#include <algorithm>

namespace penciloid
{
namespace akari
{
Problem::Problem() : grid_()
{
}
Problem::Problem(Y height, X width) : grid_(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			SetClue(CellPosition(y, x), kEmpty);
		}
	}
}
Problem::Problem(Y height, X width, const char* clues[]) : grid_(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= clues[y][x] && clues[y][x] <= '3') {
				SetClue(CellPosition(y, x), Clue(clues[y][x] - '0'));
			} else if (clues[y][x] == '#') {
				SetClue(CellPosition(y, x), kBlock);
			} else {
				SetClue(CellPosition(y, x), kEmpty);
			}
		}
	}
}
}
}
