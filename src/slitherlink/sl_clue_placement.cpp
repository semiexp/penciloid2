#include "sl_clue_placement.h"

#include <algorithm>

namespace penciloid
{
namespace slitherlink
{
CluePlacement::CluePlacement() : Grid<Clue>()
{
}
CluePlacement::CluePlacement(Y height, X width) : Grid<Clue>(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			SetClue(Position(y, x), kNoClue);
		}
	}
}
CluePlacement::CluePlacement(Y height, X width, const char* clues[]) : Grid<Clue>(height, width)
{
	for (Y y(0); y < height; ++y) {
		for (X x(0); x < width; ++x) {
			if ('0' <= clues[y][x] && clues[y][x] <= '3') {
				SetClue(Position(y, x), Clue(clues[y][x] - '0'));
			} else if (clues[y][x] == '?') {
				SetClue(Position(y, x), kSomeClue);
			} else {
				SetClue(Position(y, x), kNoClue);
			}
		}
	}
}
}
}
