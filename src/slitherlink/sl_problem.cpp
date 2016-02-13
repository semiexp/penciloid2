#include "sl_problem.h"

#include <algorithm>

namespace penciloid
{
namespace slitherlink
{
Problem::Problem() : Grid<Clue>()
{
}
Problem::Problem(Y height, X width) : Grid<Clue>(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			SetClue(CellPosition(y, x), kNoClue);
		}
	}
}
Problem::Problem(Y height, X width, const char* clues[]) : Grid<Clue>(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= clues[y][x] && clues[y][x] <= '3') {
				SetClue(CellPosition(y, x), Clue(clues[y][x] - '0'));
			} else {
				SetClue(CellPosition(y, x), kNoClue);
			}
		}
	}
}
}
}
