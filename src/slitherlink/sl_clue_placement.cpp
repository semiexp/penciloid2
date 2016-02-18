#include "sl_clue_placement.h"

#include <algorithm>

namespace penciloid
{
namespace slitherlink
{
CluePlacement::CluePlacement() : grid_()
{
}
CluePlacement::CluePlacement(Y height, X width) : grid_(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			SetClue(CellPosition(y, x), kNoClue);
		}
	}
}
CluePlacement::CluePlacement(Y height, X width, const char* clues[]) : grid_(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= clues[y][x] && clues[y][x] <= '3') {
				SetClue(CellPosition(y, x), Clue(clues[y][x] - '0'));
			} else if (clues[y][x] == '?') {
				SetClue(CellPosition(y, x), kSomeClue);
			} else {
				SetClue(CellPosition(y, x), kNoClue);
			}
		}
	}
}
}
}
