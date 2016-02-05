#pragma once

#include "../common/grid.h"
#include "sl_type.h"

namespace penciloid
{
namespace slitherlink
{
class CluePlacement : Grid<Clue>
{
public:
	CluePlacement();

	// Initialize a clue placement with the empty board
	CluePlacement(Y height, X width);

	// Set the clue placement according to <clues>.
	// '0', '1', '2' and '3' are considered a clue of the corresponding number.
	// '?' is considered as "some clue".
	// Other characters stand for cells without clues.
	CluePlacement(Y height, X width, const char* clues[]);

	~CluePlacement() {}

	inline void SetClue(Position cell, Clue clue) { at(cell) = clue; }
	inline Clue GetClue(Position cell) const { return at(cell); }
};
}
}